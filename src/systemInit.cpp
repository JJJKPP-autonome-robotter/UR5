#include "../headers/systemInit.hpp"

using namespace std;

// Global variables
ConfigFile cfg("../config.yaml");
RobotArm *ur5;
Gripper *gripper;
CaptureImage camera(cfg.get<int>("cvCfg", "cameraPort"));
PixelToRobot *pixelToRobot;
unordered_map<string, vector<double>> dropPoints;

// debug
bool DEBUG = cfg.get<bool>("cvCfg", "debug");

void initializeRobot() {
    // Get robot IP
    string ip = cfg.get<string>("robotCfg", "robotIp");
    cout << "Enter robot ip: " << ip << endl;

    // Robot arm variables
    double velocity = cfg.get<double>("robotCfg", "velocity");
    double acceleration = cfg.get<double>("robotCfg", "acceleration");
    double dt = cfg.get<double>("robotCfg", "dt");
    double lookaheadTime = cfg.get<double>("robotCfg", "lookaheadTime");
    double gain = cfg.get<double>("robotCfg", "gain");
    vector<double> basePos = cfg.get<vector<double>>("robotCfg", "basePos");

    // Gripper variables
    uint32_t gripperPortBaudrate = cfg.get<uint32_t>("gripperCfg", "baudrate");
    vector<double> tcpOffset = cfg.get<vector<double>>("gripperCfg", "tcpOffset");
    double payload = cfg.get<double>("gripperCfg", "payload");

    // Initialize gripper
    gripper = new Gripper(gripperPortBaudrate, tcpOffset, payload);

    // Initialize robot and connect
    ur5 = new RobotArm(ip, velocity, acceleration, dt, lookaheadTime, gain, basePos, gripper);
    ur5->connect();
}

void calibrateSystem() {
    // Initialize PixelToRobot
    string imagePath = cfg.get<string>("cvCfg", "imagePath");
    pixelToRobot = new PixelToRobot(imagePath);
    pixelToRobot->calibrate(&cfg);

    if (DEBUG) {
        pixelToRobot->showResults();
    }

    // Set reference points
    vector<double> refPoint1 = cfg.get<vector<double>>("robotCfg", "refPoint1");
    vector<double> refPoint2 = cfg.get<vector<double>>("robotCfg", "refPoint2");
    vector<double> refPoint3 = cfg.get<vector<double>>("robotCfg", "refPoint3");
    ur5->setRefPoints(refPoint1, refPoint2, refPoint3);

    // Validate reference points
    char in;
    cout << "Want to validate ref points y/n: ";
    cin >> in;
    if (in == 'y') {
        ur5->validateRefPoints();
        refPoint1 = ur5->getRefPoint1();
        refPoint2 = ur5->getRefPoint2();
        refPoint3 = ur5->getRefPoint3();
        cfg.set<vector<double>>("robotCfg", "refPoint1", refPoint1);
        cfg.set<vector<double>>("robotCfg", "refPoint2", refPoint2);
        cfg.set<vector<double>>("robotCfg", "refPoint3", refPoint3);
        cfg.save();
    }

    // Compute transformation matrix
    pixelToRobot->computeTransformation(refPoint1, refPoint2, refPoint3);

    // Set drop points
    dropPoints = cfg.get<unordered_map<string, vector<double>>>("robotCfg", "dropPoints");
    ur5->setDropPoints(dropPoints);

    // Validate drop points
    cout << "Want to validate drop points y/n: ";
    cin >> in;
    if (in == 'y') {
        ur5->validateDropPoints();
        dropPoints = ur5->getDropPoints();
        cfg.set<unordered_map<string, vector<double>>>("robotCfg", "dropPoints", dropPoints);
        cfg.save();
    }
}


pair<Point, string> captureAndProcess(const vector<string> &selectedColors) {
    // Capture and save image
    string imagePath = cfg.get<string>("cvCfg","imagePath");
    if (camera.captureAndSave(imagePath)) {
        cout << "Image successfully captured and saved!" << endl;
    } else {
        cerr << "Failed to capture image." << endl;
        return {Point(0, 0), ""};  // Indicate failure
    }

    // Process image
    string imagePath = cfg.get<string>("cvCfg", "imagePath");

    ProcessImage processor(imagePath);
    processor.setHsvRange(&cfg);

    pair<Point, string>
    toPick = processor.detectAll(selectedColors);
    vector<pair<Point, string>> centers = processor.getCenters();


    if (DEBUG){
        cout << "Number of detected centers: " << centers.size() << endl;
        for (const auto &center : centers) {
            cout << "Color: " << center.second
                 << ", Coordinates: (" << center.first.x << ", " << center.first.y << ")" << endl;
        }

        processor.showResults();  // Debug
    }
 
    return toPick; // return point and color
}

DataLogger initDataLogger() {
    string dbFile = cfg.get<string>("dataLogger","dbFileName");
    DataLogger db = DataLogger(dbFile);
    return db;
}
