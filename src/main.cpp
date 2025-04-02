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

using namespace std; // Ensure we use std namespace

int main(){
	ConfigFile cfg("../config.yaml");

	// Get robot ip
	string ip = cfg.get<string>("robotCfg", "robotIp");
	cout << "Enter robot ip: " << ip << endl;

	// Robot arm variables
	double velocity = cfg.get<double>("robotCfg", "velocity");				 // Standart velocity
	double acceleration = cfg.get<double>("robotCfg", "acceleration");		 // Standart acceleration
	double dt = cfg.get<double>("robotCfg", "dt");							 // Standart update rate
	double lookaheadTime = cfg.get<double>("robotCfg", "lookaheadTime");	 // Standart lookahead time
	double gain = cfg.get<double>("robotCfg", "gain");						 // Standard gain
	vector<double> basePos = cfg.get<vector<double>>("robotCfg", "basePos"); // base position for program

	// Gripper variables
	uint32_t gripperPortBaudrate = cfg.get<uint32_t>("gripperCfg", "baudrate");
	vector<double> tcpOffset = cfg.get<vector<double>>("gripperCfg", "tcpOffset");
	double payload = cfg.get<double>("gripperCfg", "payload");

	// Init gripper
	Gripper *gripper = new Gripper(gripperPortBaudrate, tcpOffset, payload);

	// Init robot and connect
	RobotArm ur5 = RobotArm(ip, velocity, acceleration, dt, lookaheadTime, gain, basePos, gripper);
	ur5.connect();

	// init camara
	CaptureImage camera(4);

	// take first image
	if (camera.captureAndSave("input.jpg"))	{
		cout << "Image successfully captured and saved!" << endl;
	}
	else{
		cerr << "Failed to capture image." << endl;
	}

	string imagePath = cfg.get<string>("cvCfg", "imagePath"); // get path to image

	PixelToRobot pixelToRobot(imagePath); // init pixelToRobot Class

	pixelToRobot.calibrate();	// find cali points
	pixelToRobot.showResults(); // DEBUG and show them.

	// Calibrating robot
	// Set Refpoints
	vector<double> refPoint1 = cfg.get<vector<double>>("robotCfg", "refPoint1");
	vector<double> refPoint2 = cfg.get<vector<double>>("robotCfg", "refPoint2");
	vector<double> refPoint3 = cfg.get<vector<double>>("robotCfg", "refPoint3");
	ur5.setRefPoints(refPoint1, refPoint2, refPoint3);

	// Ask for validation of ref points
	char in;
	cout << "Want to validate ref points y/n: ";
	cin >> in;
	if (in == 'y')	{
		ur5.validateRefPoints();
		refPoint1 = ur5.getRefPoint1();
		refPoint2 = ur5.getRefPoint2();
		refPoint3 = ur5.getRefPoint3();
		cfg.set<vector<double>>("robotCfg", "refPoint1", refPoint1);
		cfg.set<vector<double>>("robotCfg", "refPoint2", refPoint2);
		cfg.set<vector<double>>("robotCfg", "refPoint3", refPoint3);
		cfg.save();
	}

	// Set the transformation matrix
	pixelToRobot.computeTransformation(refPoint1, refPoint2, refPoint3); // takes 3 cali points and makes trans-matrix

	// Set Drop points
	unordered_map<string, vector<double>> dropPoints = cfg.get<unordered_map<string, vector<double>>>("robotCfg", "dropPoints");
	ur5.setDropPoints(dropPoints);

	// Ask for validation of ref points
	cout << "Want to validate drop points y/n: ";
	cin >> in;
	if (in == 'y')	{
		ur5.validateDropPoints();
		dropPoints = ur5.getDropPoints();
		cfg.set<unordered_map<string, vector<double>>>("robotCfg", "dropPoints", dropPoints);
		cfg.save();
	}

	vector<string> selectedColors = {"red", "orange"}; // colors to detect

	// main loop
	while (true)	{

		// takes new image
		if (camera.captureAndSave("input.jpg"))	{
			cout << "Image successfully captured and saved!" << endl;
		}
		else{
			cerr << "Failed to capture image." << endl;
		}

		//
		ProcessImage processor(imagePath);
		pair<Point, string> toPick = processor.detectAll(selectedColors); // gets the first point and color

		Point mmCenter = toPick.first;
		string color = toPick.second;

		vector<pair<Point, string>> centers = processor.getCenters(); // get all centers
		
		// print number of detected centers
		cout << "Number of detected centers: " << centers.size() << endl << endl;


		// print all centers
		cout << "Detected M&M centers:" << endl;
		for (const auto &center : centers)	{
			cout << "Color: " << center.second << ", Coordinates: (" << center.first.x << ", " << center.first.y << ")" << endl;
		}
		cout << endl;

		processor.showResults(); // DEBUG

		// beregn koordinater
		refPoint1 = ur5.getRefPoint1();
		refPoint2 = ur5.getRefPoint2();
		refPoint3 = ur5.getRefPoint3();

		for (int i = 0; i < 6; i++)	{
			cout << refPoint1[i] << ", ";
		}
		cout << endl;
		for (int i = 0; i < 6; i++)	{
			cout << refPoint2[i] << ", ";
		}
		cout << endl;
		for (int i = 0; i < 6; i++)	{
			cout << refPoint3[i] << ", ";
		}
		cout << endl;

		// Calculate the transformation matrix
		Point2f robotCoord = pixelToRobot.transformPoint(mmCenter);


		cout << "Converted coords" << endl;	
		// Output result
		cout << "Pixel (" << mmCenter.x << ", " << mmCenter.y << ") -> "
			 << "Robot (" << robotCoord.x << ", " << robotCoord.y << ")" << endl;

		vector<double> mm = {robotCoord.x, robotCoord.y};

		ur5.pickUp(color, mm);
	}

	return 0;
}