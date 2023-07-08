#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <curl/curl.h>
#include "GPS/GPS.hpp"
#include "TimerUtils.hpp"
#include "GPSLogger.hpp"
#include "MovingAverageFilter.hpp"
#include "Configs.hpp"

// Main Timer Timeouts
#define SLEEP_TIMEOUT   600  // 10 min
#define IDLE_TIMEOUT    250  // 2.5 min 
#define NORMAL_TIMEOUT  5

// State Timer Timeouts
#define TIMEOUT_STATE   300 // extra second on these

std::string URL_FINAL = "";


enum LogStates{
    SLEEP,
    IDLE,
    NORMAL
};


bool sendData(const GPS::GPSData& data) {
    std::string url = URL_FINAL
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

void attemptGPSSend(const GPS::GPSData& data)
{
    static std::string logfile = Configs::getInstance()["log_file"];

    if(!sendData(data)){
        GPSLogger::logData_Single(data);
    }

    GPSLogger::logData(logfile, data);
}

void attemptGPSResend(){
    GPS::GPSData old_gps_data;

    if(GPSLogger::readOldestData(old_gps_data)){
        if(sendData(old_gps_data)){
            GPSLogger::deleteOldestData();
        }
    }
}

int main(){
    LogStates state = NORMAL;
    TimerUtils::Timer main_timer(NORMAL_TIMEOUT);
    TimerUtils::Timer state_timer(TIMEOUT_STATE);

    MovingAverageFilter state_speed_filter(10);

    Configs& config = Configs::getInstance();
    config.load("configs.txt");

    URL_FINAL = config["traccar_url"] + config["device_id"];

    GPS gps;
    gps.connect();

    GPS::GPSData gps_data;

    bool run = true;
    while(run)
    {
        if(gps.getData(gps_data)){

            state_speed_filter.pushSample(gps_data.kmh);
            if(state_speed_filter.average() > 1){
                    state_timer.reset(TIMEOUT_STATE);
                    state = NORMAL;
                }
                if(state_timer.expired()){
                    switch(state){
                        case SLEEP:
                        case IDLE:
                            state = SLEEP;
                            break;
                        case NORMAL:
                            state = IDLE;
                            break;
                        default:
                            state = NORMAL;
                            break;
                    }
                    state_timer.reset(TIMEOUT_STATE);
                }

            if(main_timer.expired()){
                
                attemptGPSSend(gps_data);
                //std::cout<<gps_data.timestamp<<"   "<<gps_data.lat<<"   "<<gps_data.lng<<"   "<<gps_data.alt<<"   kmh: "<<gps_data.kmh<<"   "<<gps_data.hdop<<std::endl;

                switch(state)
                {
                    case SLEEP:
                        main_timer.reset(SLEEP_TIMEOUT);
                        break;
                    case IDLE:
                        main_timer.reset(IDLE);
                        break;
                    case NORMAL:
                        main_timer.reset(NORMAL);
                        break;
                    default:
                        main_timer.reset(NORMAL);
                        break;
                }
            }

        }

        attemptGPSResend();

       // std::cout<<"State: "<<(int)state<<std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    gps.disconnect();
    return 0;
}