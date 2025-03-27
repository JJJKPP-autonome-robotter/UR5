#include "../headers/Data_saver.hpp"

ConfigFile::ConfigFile() {}
ConfigFile::~ConfigFile() {}

ConfigFile::ConfigFile(string _fileName) {
    fileName = _fileName;

    try {
        config = YAML::LoadFile(fileName);
    } catch (const YAML::Exception& e) {
        cerr << "Error loading config file: " << e.what() << endl;
    }
}

void ConfigFile::save() const {
    ofstream fout(fileName);
    if (!fout) {
        cerr << "Error saving config file: " << fileName << endl;
    }
    fout << config;
}