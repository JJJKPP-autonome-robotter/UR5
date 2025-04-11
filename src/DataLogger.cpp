#include "../headers/DataLogger.hpp"

DataLogger::DataLogger() : db(nullptr), runId(-1) {}
DataLogger::~DataLogger() {
    if (db) {
        if (sqlite3_get_autocommit(db) == 0) {
            commitTransaction();
        }

        sqlite3_close(db);
    }
}

DataLogger::DataLogger(const string& dbFile) {
    dbFileName = dbFile;
    db = nullptr;
    runId = -1;

    if (sqlite3_open(dbFileName.c_str(), &db)) {
        cerr << "Can't open data base: " << sqlite3_errmsg(db) << endl;
        db = nullptr;

    } else {
        cout << "Succesfully opened database: " << dbFileName << endl;

        if (createTables()) {
            startNewRun();

        } else {
            cerr << "Failed to create required tables" << endl;
        }
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
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, eventTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Failed to create event table: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    cout << "Tables created successfully" << endl;
    return true;
}

bool DataLogger::startNewRun() {
    if (!db) {
        cerr << "Database connection is null" << endl;
        return false;
    }

    time_t now = time(0);
    char timeStamp[100];
    strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    string sql = "INSERT INTO runs (start_time) VALUES (?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement for new run: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, timeStamp, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Failed to insert new run: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);

    runId = static_cast<int>(sqlite3_last_insert_rowid(db));
    cout << "Started new run: ID = " << runId << endl;
    return true;
}

vector<unsigned char> DataLogger::encodeImage(const string &path) {
    cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);

    if (image.empty()) {
        cerr << "Warning: Cannot open image: " << path << endl;
        return vector<unsigned char>();
    }

    vector<unsigned char> buffer;
    cv::imencode(".jpg", image, buffer);
    return buffer;
}

vector<unsigned char> DataLogger::encodeMask(const cv::Mat& mask) {
    if (mask.empty()) {
        cerr << "Warning: Empty mask provided" << endl;
        return vector<unsigned char>();
    }

    vector<unsigned char> buffer;
    cv::imencode(".jpg", mask, buffer);
    return buffer;
}

string DataLogger::vectorToString(const vector<double>& vector) {
    ostringstream oss;
    for (size_t i = 0; i < vector.size(); ++i) {
        oss << vector[i];
        if (i != vector.size() - 1) oss << ", ";
    }

    return oss.str();
}





void DataLogger::beginTransaction() {
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
}

void DataLogger::commitTransaction() {
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
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

    string sql = "INSERT INTO pick_events (run_id, timeStamp, color, realPos, picPos, image, mask) "
                "VALUES (?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);


    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(stmt, 1, runId);
    sqlite3_bind_text(stmt, 2, timeStamp, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, color.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, realCords.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, picCords.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 6, imageBlob.data(), imageBlob.size(), SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 7, maskBlob.data(), maskBlob.size(), SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Insert failed (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_finalize(stmt);

    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Failed to commit transaction: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}
