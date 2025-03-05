#include "../headers/Robot_arm.hpp"
#include "../headers/Gripper.hpp"

#include <iostream>
#include <string>
#include <array>
#include <vector>

using namespace std;

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
    vector<double> base_pos = {0.23908, -0.34586, 0.48748, 3.14, 0, 0}; // base position for program

	// Get griper serial port
	string gripper_port;
	// Make function which finds the port the pico is connected to
	// And sends it to the griper class constructor
	gripper_port = find_port();
	
	// Gripper variables
	uint32_t gripper_port_baudrate = 115200;

	

	// Init gripper
	Gripper* gripper = new Gripper(gripper_port, gripper_port_baudrate);

	// Init robot and connect
	Robot_arm ur5 = Robot_arm(ip, velocity, acceleration, dt, lookahead_time, gain, base_pos, gripper);
	ur5.connect();

	// Calibrating robot 
	// Set Refpoints
	vector<double> ref_point_1 = {-0.09295, -0.42590, 0, 3.14, 0, 0};
	vector<double> ref_point_2 = {0.32133, -0.24458, 0, 3.14, 0, 0};
	ur5.set_ref_points(ref_point_1, ref_point_2);

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

	vector<string> test_colors = {"red", "orange", "yellow", "green", "blue", "brown", "yellow", "green"};

	vector<vector<double>> test_points = {
		{0.32520, -0.41333},
		{0.25335, -0.46077},
		{0.10426, -0.51534},
		{0.25325, -0.56035},
		{0.15228, -0.40742},
		{0.21080, -0.59934},
		{0.04320, -0.54633},
		{0.31110, -0.45123}
	};


	for (int i = 0; i < test_colors.size(); i++) {
		string color = test_colors[i];
		vector<double> point = test_points[i];

		ur5.pick_up(color, point);
	}
	// Launch GUI, take user input
	
	// Main Loop going until finished.
}
