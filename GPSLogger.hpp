#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm> 
#include <vector>
#include "GPS/GPS.hpp"

class GPSLogger
{
public:
    GPSLogger();

    static void logData(std::string filename, GPS::GPSData& data);

    static void logData_Single(std::string dir, GPS::GPSData data);

    static bool readOldestData(std::string dir, GPS::GPSData& data);

private:
    //std::string filename;
};