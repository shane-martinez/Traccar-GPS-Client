#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <curl/curl.h>
#include "GPS/GPS.hpp"
#include "TimerUtils.hpp"
#include "GPSLogger.hpp"
#include "Configs.hpp"

#define SLEEP_TIMEOUT   600  // 10 min
#define IDLE_TIMEOUT    250  // 2.5 min 
#define NORMAL_TIMEOUT  5

const std::string SERVER_FILE = "server-params";

const std::string log_dir     = "/home/patrol/Traccar_gps/Traccar-GPS-Client/logs/";
const std::string unsent_dir  = "";
const std::string log_file    = "log_file.csv";


enum LogStates{
    SLEEP,
    IDLE,
    NORMAL
};


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

    Configs& config = Configs::getInstance();
    config.load("config.txt");

    std::string URL = config["traccar_url"] + config["device_id"];

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

                        //GPSLogger::logData_Single(log_dir, gps_data);
                        GPSLogger::readOldestData(gps_data);
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