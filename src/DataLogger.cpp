#include "../headers/DataLogger.hpp"

DataLogger::DataLogger() {}
DataLogger::~DataLogger() {
    if (db) {
        sqlite3_close(db);
    }
}

DataLogger::DataLogger(const string& dbFile) {
    dbFileName = dbFile;

    if (sqlite3_open(dbFileName.c_str(), &db)) {
        cerr << "Can't open data base: " << sqlite3_errmsg(db) << endl;
        db = nullptr;
    } else {
        if (createTables()) startNewRun();
    }
}

bool DataLogger::createTables() {
    const char* runTable = R"(
        CREATE TABLE IF NOT EXISTS runs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            start_time TEXT NOT NULL
        );
    )";

    const char* eventTable = R"(
        CREATE TABLE IF NOT EXISTS pick_events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            run_id INTEGER NOT NULL,
            timeStamp TEXT NOT NULL,
            color TEXT,
            realPos TEXT,
            picPos TEXT,
            FOREIGN KEY(run_id) REFERENCES runs(id)
        );
    )";

    char* errMsg = nullptr;

    if (sqlite3_exec(db, runTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Failed to create runs table: " << errMsg << endl;
        return false;
    }
    if (sqlite3_exec(db, eventTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Failed to create event table: " << errMsg << endl;
        return false;
    }

    return true;
}

bool DataLogger::startNewRun() {
    time_t now = time(0);
    char timeStamp[100];
    strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    string sql = "INSERT INTO runs (start_time) VALUES ('" + string(timeStamp) + "');";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Failed to insert new run: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    runId = static_cast<int>(sqlite3_last_insert_rowid(db));
    cout << "Started new run: ID = " << runId << endl;
    return true;
}

string DataLogger::vectorToString(const vector<double>& vector) {
    ostringstream oss;
    for (size_t i = 0; i < vector.size(); ++i) {
        oss << vector[i];
        if (i != vector.size() - 1) oss << ", ";
    }

    return oss.str();
}

bool DataLogger::logEvent(const string &color, const vector<double> &realCord, const vector<double> &picCord) {
    if (!db || runId <= 0) return false;

    time_t now = time(0);
    char timeStamp[100];
    strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    string realCords = vectorToString(realCord);
    string picCords = vectorToString(picCord);

    string sql = "INSERT INTO pick_events (timeStamp, color, realCordX, realCordY, picCordX, picCordY) VALUES ('" + 
        string(timeStamp) + "', '" + color + "', '" + realCords + "', '" + picCords + ");";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Insert failed: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}
