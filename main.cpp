#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <curl/curl.h>
#include "GPS/GPS.hpp"
#include "TimerUtils.hpp"
#include "GPSLogger.hpp"

#define SLEEP_TIMEOUT   600  // 10 min
#define IDLE_TIMEOUT    250  // 2.5 min 
#define NORMAL_TIMEOUT  5

//const std::string TRACCAR_URL = "http://192.168.1.117:5055/?id=";  //"http://demo.traccar.org:5055/?id=";
//const std::string CAR_ID      = "rd28_1997";
const std::string SERVER_FILE = "server-params";

const std::string log_dir     = "";
const std::string unsent_dir  = "";
const std::string log_file    = "log_file.csv";


enum LogStates{
    SLEEP,
    IDLE,
    NORMAL
};

void readServerParams(std::string& traccarUrl, std::string& carId) {
    std::ifstream file(SERVER_FILE);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                // Trim leading/trailing whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                if (key == "url") {
                    traccarUrl = value;
                } else if (key == "id") {
                    carId = value;
                }
            }
        }
        file.close();
    } else {
        std::cerr << "Unable to open server-params file." << std::endl;
    }
}

bool sendData(const std::string S_URL, const GPS::GPSData& data) {
    std::string url = S_URL
                      + "&timestamp=" + std::to_string(data.timestamp)
                      + "&lat=" + std::to_string(data.lat)
                      + "&lon=" + std::to_string(data.lng)
                      + "&altitude=" + std::to_string(data.alt)
                      + "&speed=" + std::to_string(data.kmh) // speed needs to be check for correct (server was showing 18kmh when sending 10kmh)
                      + "&hdop=" + std::to_string(data.hdop);
                      
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return false;
        }
        curl_easy_cleanup(curl);
        return true;
    }
    return false;
}

int main(){
    LogStates state = NORMAL;
    TimerUtils::Timer main_timer(NORMAL_TIMEOUT);
    std::string tracc_url, id;
    readServerParams(tracc_url, id);
    const std::string URL = tracc_url + id;

    GPS gps;
    gps.connect();

    GPS::GPSData gps_data;

    bool run = true;
    while(run)
    {
        if(gps.getData(gps_data)){
            //std::cout<<gps_data.timestamp<<"   "<<gps_data.lat<<"   "<<gps_data.lng<<"   "<<gps_data.alt<<"   "<<gps_data.kmh<<"   "<<gps_data.hdop<<std::endl;
            // log and attempt to send latest data
            switch(state)
            {
                case SLEEP:
                    if(main_timer.expired())
                    {
                        main_timer.reset(SLEEP_TIMEOUT);


                    }
                    break;
                case IDLE:
                    if(main_timer.expired())
                    {
                        main_timer.reset(IDLE_TIMEOUT);


                    }
                    break;
                case NORMAL:
                    if(main_timer.expired())
                    {
                        main_timer.reset(NORMAL_TIMEOUT);

                        sendData(URL, gps_data);
                    }
                    break;
                default:
                    break;
            }
        }

        // send old data that has not been sent yet

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    gps.disconnect();
    return 0;
}