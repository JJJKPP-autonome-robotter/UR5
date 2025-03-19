#include "../headers/captureImage.hpp"
#include "../headers/processImage.hpp"
#include "../headers/PixelToRobot.hpp"
#include "../headers/Robot_arm.hpp"
#include "../headers/Gripper.hpp"

#include <iostream>
#include <string>
#include <array>
#include <vector>

using namespace std;  // Ensure we use std namespace


string imagePath = "input.jpg";  

int main() {
	// Get robot ip
	string ip;
	cout << "Enter robot ip: ";
	cin >> ip;

	// Robot arm variables
    double velocity = 0.25; // Standart velocity
    double acceleration = 0.25; // Standart acceleration
    double dt = 1.0/500; // Standart update rate
    double lookahead_time = 0.1; // Standart lookahead time
    double gain = 300; // Standard gain
    vector<double> base_pos = {0.03639, -0.23713, 0.43206, 3.14, 0, 0}; // base position for program
	
	// Gripper variables
	uint32_t gripper_port_baudrate = 115200;
	vector<double> tcp_offset = {0, 0, 0.1, 0, 0, 0};

	// Init gripper
	Gripper* gripper = new Gripper(gripper_port_baudrate, tcp_offset);

	// Init robot and connect
	Robot_arm ur5 = Robot_arm(ip, velocity, acceleration, dt, lookahead_time, gain, base_pos, gripper);
	ur5.connect();

	// Calibrating robot 
	// Set Refpoints
	vector<double> ref_point_1 = {-0.09295, -0.42590, 0, 3.14, 0, 0};
	vector<double> ref_point_2 = {0.32133, -0.24458, 0, 3.14, 0, 0};
	vector<double> ref_point_3 = {0.32133, -0.24458, 0, 3.14, 0, 0};
	ur5.set_ref_points(ref_point_1, ref_point_2, ref_point_3);

	// Ask for validation of ref points
	char in;
	cout << "Want to validate ref points y/n: ";
	cin >> in;
	if (in == 'y') {
		ur5.validate_ref_points();
	}

	// Set Drop points
	unordered_map<string, vector<double>> drop_points = {
		{"red", {0.04479, -0.82136, 0.2, 3.14, 0, 0}},
		{"orange", {0.13456, -0.78130, 0.2, 3.14, 0, 0}},
		{"yellow", {0.22856, -0.74187, 0.2, 3.14, 0, 0}},
		{"green", {0.31588, -0.70640, 0.2, 3.14, 0, 0}},
		{"blue", {0.41081, -0.66906, 0.2, 3.14, 0, 0}},
		{"brown", {0.50244, -0.62575, 0.2, 3.14, 0, 0}}
	};
	ur5.set_drop_points(drop_points);

	// Ask for validation of ref points
	cout << "Want to validate drop points y/n: ";
	cin >> in;
	if (in == 'y') {
		ur5.validate_drop_points();
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
   
    Point2f testPixel = centers[0];

   // beregn koordinater
    PixelToRobot pixelToRobot(imagePath);

	ref_point_1 = ur5.get_ref_point_1();
	ref_point_2 = ur5.get_ref_point_2();
	ref_point_3 = ur5.get_ref_point_3();

	for (int i = 0; i < 6; i++) {
		cout << ref_point_1[i] << ", ";
	}
	cout << endl;
	for (int i = 0; i < 6; i++) {
		cout << ref_point_2[i] << ", ";
	}
	cout << endl;
	for (int i = 0; i < 6; i++) {
		cout << ref_point_3[i] << ", ";
	}
	cout << endl;

    vector<Point2f> robot_points = {
        Point2f(ref_point_1[0], ref_point_1[1]),
        Point2f(ref_point_2[0], ref_point_2[1]),
        Point2f(ref_point_3[0], ref_point_3[1])
    };

    pixelToRobot.calibrate(robot_points);

    pixelToRobot.showResults(); // DEBUG
    Point2f robotCoord = pixelToRobot.transformPoint(testPixel);

    // Output result
    cout << "Pixel (" << testPixel.x << ", " << testPixel.y << ") -> "
        << "Robot (" << robotCoord.x << ", " << robotCoord.y << ")" << endl;
	// Launch GUI, take user input
	
	// Main Loop going until finished.
}
