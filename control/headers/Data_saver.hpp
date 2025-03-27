#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#pragma once

#include <iostream>
#include <yaml-cpp/yaml.h>
#include <fstream>

using namespace std;

class ConfigFile {
public:
    ConfigFile();
    ~ConfigFile();
    explicit ConfigFile(string);

    template<typename T>
    T get(string key, string key1) const {
        if (config[key][key1]) return config[key][key1].as<T>();
        throw runtime_error("Config file is missing: " + key);
    }

    template<typename T>
    void set(string key, string key1, const T& value) {
        config[key][key1] = value;
    }

    void save() const;

private:
    string fileName;
    YAML::Node config;

};

#endif