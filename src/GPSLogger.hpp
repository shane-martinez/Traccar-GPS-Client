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

    static void logData(const std::string filename, const GPS::GPSData& data);

    static void logData_Single(const GPS::GPSData& data);

    static bool readOldestData(GPS::GPSData& data);
    
    static bool deleteOldestData();

private:
    //std::string filename;

    static void deleteFile(std::string filename);
};