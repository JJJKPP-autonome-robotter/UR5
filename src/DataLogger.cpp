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

DataLogger::DataLogger(ConfigFile* _cfg) {
    cfg = _cfg;
    dbFileName = cfg->get<string>("dataLogger","dbFileName");
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
            pickup BOOLEAN,
            realPos TEXT,
            picPos TEXT,
            hsvLower TEXT,
            hsvUpper TEXT,
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
    try {
        cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);

        if (image.empty()) {
            cerr << "Warning: Cannot open image: " << path << endl;
            return vector<unsigned char>();
        }

        vector<unsigned char> buffer;
        cv::imencode(".jpg", image, buffer);
        return buffer;
    } catch (const exception& e) {
        cerr << "Error encoding image: " << e.what() << endl;
    }

    return vector<unsigned char>();
}

vector<unsigned char> DataLogger::encodeMask(const cv::Mat& mask) {
    try {
        if (mask.empty()) {
            cerr << "Warning: Empty mask provided" << endl;
            return vector<unsigned char>();
        }

        vector<unsigned char> buffer;
        cv::imencode(".jpg", mask, buffer);
        return buffer;
    } catch (const exception& e) {
        cerr << "Error encoding mask: " << e.what() << endl;
    }

    return vector<unsigned char>();
}

string DataLogger::vectorToString(const vector<double>& vector) {
    ostringstream oss;
    for (size_t i = 0; i < vector.size(); ++i) {
        oss << vector[i];
        if (i != vector.size() - 1) oss << ", ";
    }

    return oss.str();
}

void DataLogger::setHsvRange() {
    vector<double> lower;
    vector<double> upper;
    if (color == "red") {
        cout << "REDSAVE" << endl;
        vector<vector<double>> range1 = cfg->get<vector<vector<double>>>("color_ranges","red");
        vector<vector<double>> range2 = cfg->get<vector<vector<double>>>("color_ranges","red2");
        lower.insert(lower.end(), range1[0].begin(), range1[0].end());
        lower.insert(lower.end(), range2[0].begin(), range2[0].end());
        upper.insert(upper.end(), range1[1].begin(), range1[1].end());
        upper.insert(upper.end(), range2[1].begin(), range2[1].end());

        cout << "REDSAVEDONE" << endl;
        
    } else {
        cout << "OTHER COLOR SAVE" << endl;
        cout << color << endl;
        vector<vector<double>> range = cfg->get<vector<vector<double>>>("color_ranges", color);
        lower.insert(lower.end(), range[0].begin(), range[0].end());
        upper.insert(upper.end(), range[1].begin(), range[1].end());
        cout << "OTHER COLOR DONE" << endl;
    }

    hsvLower = vectorToString(lower);
    hsvUpper = vectorToString(upper);

    cout << "HSVLOWER: " << hsvLower << endl;
    cout << "HSVUPPER: " << hsvUpper << endl;

}

void DataLogger::beginTransaction() {
    if (!db) return;

    cout << "Auto-commit: " << sqlite3_get_autocommit(db) << endl;

    if (sqlite3_get_autocommit(db) != 0) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "Failed to begin transaction: " << errMsg << endl;
            sqlite3_free(errMsg);
        }

    }
    
}

bool DataLogger::clearVar() {
    color.clear();
    pickup = false;
    realCords.clear();
    picCords.clear();
    hsvLower.clear();
    hsvUpper.clear();
    imageBlob.clear();
    maskBlob.clear();
    return true;
}

void DataLogger::commitTransaction() {
    if (!db) return;

    if (sqlite3_get_autocommit(db) == 0) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "Failed to commit transaction: " << errMsg << endl;
            sqlite3_free(errMsg);
        }

    }

}

bool DataLogger::writeEvent() {

    if (!db) {
        cerr << "Database connection is null" << endl;
        return false;
    }
    
    if (runId <= 0) {
        cerr << "Invalid run ID: " << runId << endl;
        return false;
    }

    time_t now = time(0);
    char timeStamp[100];
    strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    cout << "Preparing to insert event:" << endl;
    cout << "- Run ID: " << runId << endl;
    cout << "- Color: " << color << endl;
    cout << "- Pickup: " << pickup << endl;
    cout << "- Real Coordinates: " << realCords << endl;
    cout << "- Pic Coordinates: " << picCords << endl;
    cout << "- HSV Lower: " << hsvLower << endl;
    cout << "- HSV Upper: " << hsvUpper << endl;
    cout << "- Image Size: " << imageBlob.size() << " bytes" << endl;
    cout << "- Mask Size: " << maskBlob.size() << " bytes" << endl;

    try {
        string sql = "INSERT INTO pick_events (run_id, timeStamp, color, pickup, realPos, picPos, hsvLower, hsvUpper, image, mask) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

        cout << "SQL: " << sql << endl;
        cout << "DB is " << (db ? "valid" : "null") << endl;

        if (!db) {
            cerr << "DB is null before preparing insert." << endl;
            return false;
        }

        cout << "sqlite3_db_filename: " << sqlite3_db_filename(db, "main") << endl;

        sqlite3_stmt* stmt = nullptr;
        cout << "Testing DB pointer: " << db << endl;
        cout << "DB filename: " << sqlite3_db_filename(db, "main") << endl;
        cout << "DB status: " << sqlite3_errmsg(db) << endl;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), static_cast<int>(sql.length()), &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "PREPARE FAILED (" << rc << "): " << sqlite3_errmsg(db) << endl;
        }

        if (rc != SQLITE_OK) {
            cerr << "Failed to prepare statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        if (sqlite3_bind_int(stmt, 1, runId) != SQLITE_OK) cerr << "Failed to bind run_id: " << sqlite3_errmsg(db) << endl;
        if (sqlite3_bind_text(stmt, 2, timeStamp, -1, SQLITE_TRANSIENT) != SQLITE_OK) cerr << "Failed to bind timeStamp: " << sqlite3_errmsg(db) << endl;
        if (sqlite3_bind_text(stmt, 3, color.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) cerr << "Failed to bind color: " << sqlite3_errmsg(db) << endl;
        if (sqlite3_bind_int(stmt, 4, pickup) != SQLITE_OK) cerr << "Failed to bind pickup: " << sqlite3_errmsg(db) << endl;
        if (sqlite3_bind_text(stmt, 5, realCords.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) cerr << "Failed to bind realPos: " << sqlite3_errmsg(db) << endl;
        if (sqlite3_bind_text(stmt, 6, picCords.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) cerr << "Failed to bind picPos: " << sqlite3_errmsg(db) << endl;
        if (sqlite3_bind_text(stmt, 7, hsvLower.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) cerr << "Failed to bind hsvLower: " << sqlite3_errmsg(db) << endl;
        if (sqlite3_bind_text(stmt, 8, hsvUpper.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) cerr << "Failed to bind hsvUpper: " << sqlite3_errmsg(db) << endl;
        if (imageBlob.empty()) {
            if (sqlite3_bind_null(stmt, 9) != SQLITE_OK) cerr << "Failed to bind null image: " << sqlite3_errmsg(db) << endl;
        } else {
            if (sqlite3_bind_blob(stmt, 9, imageBlob.data(), imageBlob.size(), SQLITE_TRANSIENT) != SQLITE_OK)
                cerr << "Failed to bind image: " << sqlite3_errmsg(db) << endl;
        }
        if (maskBlob.empty()) {
            if (sqlite3_bind_null(stmt, 10) != SQLITE_OK) cerr << "Failed to bind null mask: " << sqlite3_errmsg(db) << endl;
        } else {
            if (sqlite3_bind_blob(stmt, 10, maskBlob.data(), maskBlob.size(), SQLITE_TRANSIENT) != SQLITE_OK)
                cerr << "Failed to bind mask: " << sqlite3_errmsg(db) << endl;
        }

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            cerr << "Insert failed (" << rc << "): " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        sqlite3_finalize(stmt);

        cout << "Successfully logged pick event with ID: " << sqlite3_last_insert_rowid(db) << endl;
        return true;

    } catch (const exception& e) {
        cerr << "Exception in logEvent: " << e.what() << endl;
        return false;
    }
}

bool DataLogger::logEvent(
    const string _color,
    const bool _pickup, 
    const vector<double> _realCords, 
    cv::Point _picCords,
    const cv::Mat _mask
) {
    color = _color;
    pickup = _pickup;
    realCords = vectorToString(_realCords);
    picCords = vectorToString({static_cast<double>(_picCords.x), static_cast<double>(_picCords.y)});

    setHsvRange();

    string imagePath = cfg->get<string>("cvCfg","imagePath");
    imageBlob = encodeImage(imagePath);
    maskBlob = encodeMask(_mask);

    if (writeEvent()) {
       return clearVar();
    }
    
    return false;
}
