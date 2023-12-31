#include "GPSLogger.hpp"

// GPSLogger::GPSLogger(std::string file)
//     :filename(file)
// {

GPSLogger::GPSLogger()
{
}

void GPSLogger::logData(const std::string filename, const GPS::GPSData& data) {
    std::ofstream outfile;

    // std::ios::app is the "append" operation
    outfile.open(filename, std::ios::out | std::ios::app);

    // Check if it's the first line to print headers
    if (outfile.tellp() == 0)
        outfile << "Timestamp,Latitude,Longitude,Altitude,Speed,HDOP\n";

    outfile << data.timestamp << ','
            << data.lat << ','
            << data.lng << ','
            << data.alt << ','
            << data.kmh << ','
            << data.hdop<<'\n';

    outfile.close();
}

void GPSLogger::logData_Single(const GPS::GPSData& data) {
    static std::string dir = Configs::getInstance()["log_directory"];
    
    std::string filename = dir + std::to_string(data.timestamp) + ".csv";
    std::ofstream outfile(filename);

    // Write the data to the new file
    outfile << "Timestamp,Latitude,Longitude,Altitude,Speed,HDOP\n";
    outfile << data.timestamp << ','
            << data.lat << ','
            << data.lng << ','
            << data.alt << ','
            << data.kmh << ','
            << data.hdop << '\n';

    outfile.close();
}

// Function to read GPS data from the oldest file in a directory into a GPSData struct, then delete the file
bool GPSLogger::readOldestData(GPS::GPSData& data) {
    static std::string dir = Configs::getInstance()["log_directory"];
    // Check if directory exists
    if (!std::filesystem::exists(dir)) {
        std::cout << "Directory doesn't exist" << std::endl;
        return false;
    }

    // Find the oldest file (minimum timestamp)
    std::vector<std::string> filenames;
    for (const auto &entry : std::filesystem::directory_iterator(dir)) {
        filenames.push_back(entry.path().string());
    }
    auto oldestFile = std::min_element(filenames.begin(), filenames.end());

    if (filenames.empty()) {
        //std::cout << "Directory is empty" << std::endl;
        return false;
    }

    // Open the oldest file
    std::ifstream infile(*oldestFile);
    std::string line;

    // Skip the header line
    if (!std::getline(infile, line)) {
        std::cout << "File is empty" << std::endl;
        deleteFile(*oldestFile);
        infile.close();
        return false;
    }

    // Read the data line
    if (!std::getline(infile, line)) {
        std::cout << "No data in file" << std::endl;
        deleteFile(*oldestFile);
        infile.close();
        return false;
    }

    // Parse the CSV line into GPSData
    std::istringstream iss(line);
    char ch;  // To skip the commas

    if (!(iss >> data.timestamp >> ch
              >> data.lat >> ch
              >> data.lng >> ch
              >> data.alt >> ch
              >> data.kmh >> ch
              >> data.hdop)) {
        std::cout << "Error parsing data" << std::endl;
        infile.close();
        return false;
    }

    infile.close();

    return true;
}

bool GPSLogger::deleteOldestData(){
    static std::string dir = Configs::getInstance()["log_directory"];

    // Check if directory exists
    if (!std::filesystem::exists(dir)) {
        std::cout << "Directory doesn't exist" << std::endl;
        return false;
    }

    // Find the oldest file (minimum timestamp)
    std::vector<std::string> filenames;
    for (const auto &entry : std::filesystem::directory_iterator(dir)) {
        filenames.push_back(entry.path().string());
    }
    auto oldestFile = std::min_element(filenames.begin(), filenames.end());

    if (filenames.empty()) {
        //std::cout << "Directory is empty" << std::endl;
        return false;
    }

    deleteFile(*oldestFile);

    return true;

}

void GPSLogger::deleteFile(std::string filename){
    if (std::remove(filename.c_str()) != 0) {
        std::cout << "Error deleting file" << std::endl;
    }
}