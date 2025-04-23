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

using namespace std;

class DataLogger {
public:
    DataLogger();
    ~DataLogger();
    DataLogger(const string&);

    void beginTransaction();
    void commitTransaction();

    bool logEvent(
        const string& color, 
        const vector<double>& realCord, 
        const vector<double>& picCord,
        const string& imagePath,
        const cv::Mat& mask
    );

private:
    string dbFileName;
    sqlite3* db;
    int runId;

    bool createTables();
    bool startNewRun();
    string vectorToString(const vector<double>& vector);
    vector<unsigned char> encodeImage(const string& path);
    vector<unsigned char> encodeMask(const cv::Mat& mask);

};

#endif