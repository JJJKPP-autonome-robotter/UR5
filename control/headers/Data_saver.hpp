#ifndef DATASAVER_HPP
#define DATASAVER_HPP

#pragma once

#include <iostream>
#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

using namespace std;

using ConfigValue = std::variant<vector<double>, string, int, double, bool>;
using ConfigMap = std::unordered_map<std::string, ConfigValue>;

class DataSaver {
public:
    DataSaver();
    ~DataSaver();
    DataSaver(string);

    ConfigMap loadConfig(string);

private:
    string cfgName;

    YAML::Node cfg;

};

#endif