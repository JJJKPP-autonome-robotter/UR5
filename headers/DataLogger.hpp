#ifndef DATALOGGER_H
#define DATALOGGER_H

#pragma once

#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include "../headers/Data_saver.hpp"

using namespace std;

class DataLogger {
public:
    DataLogger();
    ~DataLogger();
    DataLogger(ConfigFile*);

    void beginTransaction();
    void commitTransaction();

    bool logEvent(const string, const bool _pickup, const vector<double>, const Point, const cv::Mat);
    bool writeEvent();

private:
    ConfigFile* cfg;
    string dbFileName;
    sqlite3* db;
    int runId;

    bool createTables();
    bool startNewRun();
    string vectorToString(const vector<double>& vector);
    vector<unsigned char> encodeImage(const string& path);
    vector<unsigned char> encodeMask(const cv::Mat& mask);
    void setHsvRange();

    const string color;
    const bool pickup;
    const string realCord;
    const string picCord;
    const string hsvLower;
    const string hsvUpper;
    vector<unsigned char> imageBlob;
    vector<unsigned char> maskBlob;

};

#endif