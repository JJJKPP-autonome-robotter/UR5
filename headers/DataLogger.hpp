#ifndef DATALOGGER_H
#define DATALOGGER_H

#pragma once

#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <vector>

using namespace std;

class DataLogger {
public:
    DataLogger();
    ~DataLogger();
    DataLogger(const string&);

    bool logEvent(const string& color, const vector<double>& realCord, const vector<double>& picCord);

private:
    string dbFileName;
    sqlite3* db;
    int runId;

    bool createTables();
    bool startNewRun();
    string vectorToString(const vector<double>& vector);

};

#endif