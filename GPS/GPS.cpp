#include "GPS.hpp"

GPS::GPS()
{

}

bool GPS::connect()
{
    std::string gps_device = "/dev/ttyUSB0";

    _fd = open(gps_device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

    if (_fd == -1) {
        char err_msg[256]; // Adjust the size to your needs
        snprintf(err_msg, sizeof(err_msg), "open_port: Unable to open USB Serial Device - %s", gps_device.c_str());
        perror(err_msg);
        return(false);
    }

    struct termios tty;
    memset (&tty, 0, sizeof(tty));

    if (tcgetattr (_fd, &tty) != 0) {
        printf ("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return false;
    }

    cfsetospeed (&tty, B115200);
    cfsetispeed (&tty, B115200);

    tty.c_cflag     &=  ~PARENB;        // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;

    tty.c_cflag     &=  ~CRTSCTS;       // no flow control
    tty.c_cc[VMIN]   =  1;              // read doesn't block
    tty.c_cc[VTIME]  =  5;              // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL; // turn on READ & ignore ctrl lines

    tcflush(_fd, TCIFLUSH);

    if (tcsetattr (_fd, TCSANOW, &tty) != 0) {
        printf ("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return false;
    }

    _run_thread = std::thread(&GPS::run, this);
	_run_thread.detach();

    return true;
}

void GPS::run()
{
    char buf [1];
    memset (&buf, '\0', sizeof(buf));

    _rungps = true;

    while(_rungps){
        int n = read (_fd, &buf , sizeof(buf));
        // If n == 0 do nothing
        if (n < 0) {
            printf ("Error %i from read: %s\n", errno, strerror(errno));
        }
        else if(n > 0){
            _gps_mutex.lock();
            // feed the _gps object 1 byte at a time
            _gps.encode(buf[0]);
            _gps_mutex.unlock();
        }
    }

    ::close(_fd);
}

void GPS::disconnect()
{
    _rungps = false;
}

bool GPS::getData(GPSData &data)
{
    if(isValid())
    {
        _gps_mutex.lock();

        data.timestamp = getUnixTime();
        data.lat       = _gps.location.lat();
        data.lng       = _gps.location.lng();
        data.alt       = _gps.altitude.meters();

        double speed = _gps.speed.kmph();
        data.kmh = (speed > 1.0) ? speed : 0.0;

        data.hdop      = _gps.hdop.hdop();

        _gps_mutex.unlock();
        
        return true;
    }

    return false;
}

unsigned long GPS::getUnixTime() {
    struct tm t;
    t.tm_year = _gps.date.year() - 1900;
    t.tm_mon = _gps.date.month() - 1;
    t.tm_mday = _gps.date.day();
    t.tm_hour = _gps.time.hour();  
    t.tm_min = _gps.time.minute();
    t.tm_sec = _gps.time.second();
    //t.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown
    return mktime(&t); // returns seconds elapsed since January 1, 1970 (i.e. the current unix timestamp)
}

std::string GPS::datetime()
{
    if(isValidDateTime())
    {
        std::ostringstream oss;
    
        oss << std::setfill('0');

        _gps_mutex.lock();

        oss << (int)_gps.date.year() << "-";
        oss << std::setw(2) << std::setfill('0') << (int)_gps.date.month() << "-";
        oss << std::setw(2) << std::setfill('0') << (int)_gps.date.day();
        oss << " ";
        oss << std::setw(2) << (int)_gps.time.hour() << ":";
        oss << std::setw(2) << (int)_gps.time.minute() << ":";
        oss << std::setw(2) << (int)_gps.time.second();

        _gps_mutex.unlock();

        return oss.str();
    }
    return "INVALID";
}

int GPS::satellites()
{
    _gps_mutex.lock();
    int sats = _gps.satellites.value();
    _gps_mutex.unlock();

    return sats;
}

double GPS::latitude()
{
    _gps_mutex.lock();
    double lat = _gps.location.lat();
    _gps_mutex.unlock();

    return lat;
}

double GPS::longitude()
{
    _gps_mutex.lock();
    double lng = _gps.location.lng();
    _gps_mutex.unlock();

    return lng;
}

double GPS::altitude()
{
    _gps_mutex.lock();
    double alt = _gps.altitude.meters();
    _gps_mutex.unlock();

    return alt;
}

double GPS::speedKMH()
{
    _gps_mutex.lock();
    double speed = _gps.speed.kmph();
    _gps_mutex.unlock();

    return speed;
}

bool GPS::isValid()
{
    return _gps.location.isValid() && _gps.date.isValid() && _gps.time.isValid();
}

bool GPS::isValidFix()
{
    return _gps.location.isValid();
}

bool GPS::isValidDateTime()
{
    return _gps.date.isValid() && _gps.time.isValid();
}
