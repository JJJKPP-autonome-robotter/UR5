#include "../headers/CaptureImage.hpp"
#include "../headers/ProcessImage.hpp"
#include "../headers/PixelToRobot.hpp"
#include "../headers/Robot_arm.hpp"
#include "../headers/Gripper.hpp"
#include "../headers/Data_saver.hpp"

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <variant>
#include <unordered_map>

using namespace std;  // Ensure we use std namespace
using ConfigValue = std::variant<vector<double>, string, int, double, bool>;
using ConfigMap = std::unordered_map<std::string, ConfigValue>;


string imagePath = "input.jpg";  


int main() {
    DataSaver config = DataSaver("../config.yaml");
    ConfigMap cfg;
    cfg = config.loadConfig("robotSettings");

	// Get robot ip
	string ip = get<string>(cfg["robotIp"]);
	cout << "Enter robot ip: " << ip << endl;

	// Robot arm variables
    double velocity = 0.25; // Standart velocity
    double acceleration = 0.25; // Standart acceleration
    double dt = 1.0/500; // Standart update rate
    double lookaheadTime = 0.1; // Standart lookahead time
    double gain = 300; // Standard gain
    vector<double> basePos = get<vector<double>>(cfg["basePos"]); // base position for program

    cout << basePos[0];
	
	// Gripper variables
	uint32_t gripperPortBaudrate = 115200;
	vector<double> tcpOffset = {0, 0, 0.216, 0, 0, 0};

	// Init gripper
	Gripper* gripper = new Gripper(gripperPortBaudrate, tcpOffset);

	// Init robot and connect
	RobotArm ur5 = RobotArm(ip, velocity, acceleration, dt, lookaheadTime, gain, basePos, gripper);
	ur5.connect();

	// Calibrating robot 
	// Set Refpoints
	vector<double> refPoint1 = {-0.09295, -0.42590, 0, 3.14, 0, 0};
	vector<double> refPoint2 = {0.32133, -0.24458, 0, 3.14, 0, 0};
	vector<double> refPoint3 = {0.32133, -0.24458, 0, 3.14, 0, 0};
	ur5.setRefPoints(refPoint1, refPoint2, refPoint3);

	// Ask for validation of ref points
	char in;
	cout << "Want to validate ref points y/n: ";
	cin >> in;
	if (in == 'y') {
		ur5.validateRefPoints();
	}

	// Set Drop points
	unordered_map<string, vector<double>> dropPoints = {
		{"red", {0.04479, -0.82136, 0.2, 3.14, 0, 0}},
		{"orange", {0.13456, -0.78130, 0.2, 3.14, 0, 0}},
		{"yellow", {0.22856, -0.74187, 0.2, 3.14, 0, 0}},
		{"green", {0.31588, -0.70640, 0.2, 3.14, 0, 0}},
		{"blue", {0.41081, -0.66906, 0.2, 3.14, 0, 0}},
		{"brown", {0.50244, -0.62575, 0.2, 3.14, 0, 0}}
	};
	ur5.setDropPoints(dropPoints);

	// Ask for validation of ref points
	cout << "Want to validate drop points y/n: ";
	cin >> in;
	if (in == 'y') {
		ur5.validateDropPoints();
	}
  
  // tager billed
    CaptureImage camera(4);
    
    if (camera.captureAndSave("input.jpg")) {
        cout << "Image successfully captured and saved!" << endl;
    } else {
        cerr << "Failed to capture image." << endl;
    }

    // 
    ProcessImage processor(imagePath);
    processor.detectRedMMS();
    processor.showResults(); // DEBUG

    vector<Point> centers = processor.getCenters();
    cout << "Detected M&M centers:" << endl;
    for (const auto& center : centers) {
        cout << "(" << center.x << ", " << center.y << ")" << endl;
    }
   
    Point2f mmCenter = centers[0];

   // beregn koordinater
    PixelToRobot pixelToRobot(imagePath);

	refPoint1 = ur5.getRefPoint1();
	refPoint2 = ur5.getRefPoint2();
	refPoint3 = ur5.getRefPoint3();

	for (int i = 0; i < 6; i++) {
		cout << refPoint1[i] << ", ";
	}
	cout << endl;
	for (int i = 0; i < 6; i++) {
		cout << refPoint2[i] << ", ";
	}
	cout << endl;
	for (int i = 0; i < 6; i++) {
		cout << refPoint3[i] << ", ";
	}
	cout << endl;

    pixelToRobot.calibrate(refPoint1, refPoint2, refPoint3);

    pixelToRobot.showResults(); // DEBUG
    Point2f robotCoord = pixelToRobot.transformPoint(mmCenter);

    // Output result
    cout << "Pixel (" << mmCenter.x << ", " << mmCenter.y << ") -> "
        << "Robot (" << robotCoord.x << ", " << robotCoord.y << ")" << endl;
	// Launch GUI, take user input
	
	// Main Loop going until finished.

    return 0;
}
