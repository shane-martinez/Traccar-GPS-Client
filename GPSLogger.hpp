#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm> 
#include <vector>
#include "GPS/GPS.hpp"
#include "Configs.hpp"

class GPSLogger
{
public:
    GPSLogger();

    static void logData(const std::string filename, GPS::GPSData& data);

    static void logData_Single(GPS::GPSData& data);

    static bool readOldestData(GPS::GPSData& data);
    
    static bool deleteOldestData(GPS::GPSData& data);

private:
    //std::string filename;
};