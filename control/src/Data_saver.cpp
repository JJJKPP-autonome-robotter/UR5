#include "../headers/Data_saver.hpp"

DataSaver::DataSaver() {}
DataSaver::~DataSaver() {}

DataSaver::DataSaver(string _cfgName) {
    cfgName = _cfgName;

    cfg = YAML::LoadFile(cfgName);

    if (!cfg) {
        throw runtime_error("Failed to load config file!");
    }
}

ConfigMap DataSaver::loadConfig(string settingsTree) {
    ConfigMap configMap;

    for (const auto& setting : cfg[settingsTree]) {
        std::string key = setting.first.as<string>();
        const YAML::Node& value = setting.second;

        cout << key << endl;

        if (value.IsScalar()) {
            if (value.Tag() == "!!int") configMap[key] = value.as<int>();
            else if (value.Tag() == "!!float") configMap[key] = value.as<double>();
            else if (value.Tag() == "!!bool") configMap[key] = value.as<bool>();
            else configMap[key] = value.as<string>();
        }
        if (value.IsSequence()) {
            if (value[0].IsScalar()) {
                cout << "Value 0 is: " << value[0].Tag() << endl;
                if (value[0].Tag() == "?") {
                    
                    configMap[key] = value.as<vector<double>>();
                } 
            }
        }
    }

    return configMap;
}