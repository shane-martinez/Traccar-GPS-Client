#pragma once
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <sstream>



class Configs {
public:
    static Configs& getInstance() {
        static Configs instance; 
        return instance;
    }

    void load(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream is_line(line);
            std::string key;

            if (std::getline(is_line, key, '=')) {
                std::string value;
                if (std::getline(is_line, value)) {
                    config_map[key] = value;
                }
            }
        }

        file.close();
    }

    void save(const std::string& filename) {
        std::ofstream file(filename);

        for (auto const& pair : config_map) {
            file << pair.first << "=" << pair.second << std::endl;
        }

        file.close();
    }

    std::string& operator[](const std::string& key) {
        return config_map[key];
    }

private:
    std::map<std::string, std::string> config_map;

    // Private constructor and destructor to prevent direct creation or deletion
    Configs() {}
    ~Configs() {}

    // Prevent copy constructor and assignment operator from being called
    Configs(const Configs&) = delete;
    void operator=(const Configs&) = delete;
};
