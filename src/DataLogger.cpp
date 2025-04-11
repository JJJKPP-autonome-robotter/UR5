#include "../headers/DataLogger.hpp"

DataLogger::DataLogger() : db(nullptr), runId(-1) {}
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
            image BLOB,
            mask BLOB,
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

vector<unsigned char> DataLogger::encodeImage(const string &path) {
    cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);

    if (image.empty()) {
        throw runtime_error("Cannot open image: " + path);
    }

    vector<unsigned char> buffer;
    cv::imencode(".jpg", image, buffer);
    return buffer;
}

vector<unsigned char> DataLogger::encodeMask(const cv::Mat& mask) {
    if (mask.empty()) throw runtime_error("Failed to load mask!");

    vector<unsigned char> buffer;
    cv::imencode(".jpg", mask, buffer);
    return buffer;
}

bool DataLogger::logEvent(
    const string& color, 
    const vector<double>& realCord, 
    const vector<double>& picCord, 
    const string& image,
    const cv::Mat& mask 
) {
    if (!db || runId <= 0) return false;

    time_t now = time(0);
    char timeStamp[100];
    strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    string realCords = vectorToString(realCord);
    string picCords = vectorToString(picCord);

    vector<unsigned char> imageBlob = encodeImage(image);
    vector<unsigned char> maskBlob = encodeMask(mask);

    string sql = R"(
        INSERT INTO pick_events (run_id, timeStamp, color, realPos, picPos, image, mask)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, runId);
    sqlite3_bind_text(stmt, 2, timeStamp, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, color.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, realCords.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, picCords.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 6, imageBlob.data(), imageBlob.size(), SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 7, maskBlob.data(), maskBlob.size(), SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Insert failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}
