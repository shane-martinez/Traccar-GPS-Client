#pragma once
#include <fcntl.h> 
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include "TinyGPS/TinyGPS++.hpp"



class GPS {
	public:

    struct GPSData{
        unsigned long timestamp;
        double lat;
        double lng;
        double alt;
        double kmh;
        double hdop;
        GPSData() = default;
    };

		GPS();

		bool connect();
		void disconnect();

        bool getData(GPSData &data);

        std::string datetime();
        int satellites();
		double latitude();
        double longitude();
        double altitude();
        double speedKMH();

        bool isValid();
		bool isValidFix();
        bool isValidDateTime();

	private:
        int _fd;
        bool _rungps = false;
        TinyGPSPlus _gps;
        std::mutex _gps_mutex;
        std::thread _run_thread;

        //unsigned long getUnixTime(int day, int month, int year, int hour, int minute, int second);
        unsigned long getUnixTime();

        void run();

};