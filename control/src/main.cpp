#include <../headers/Robot_arm.hpp>

#include <iostream>
#include <string>
#include <array>
#include <vector>

using namespace std;

int main() {
	string ip;
	cout << "Enter robot ip: ";
	cin >> ip;
    double velocity = 0.25; // Standart velocity
    double acceleration = 0.25; // Standart acceleration
    double dt = 1.0/500; // Standart update rate
    double lookahead_time = 0.1; // Standart lookahead time
    double gain = 300; // Standard gain
    vector<double> base_pos = {0.23908, -0.34586, 0.48748, 3.09, 0.439, 0.005}; // base position for program

	// Init robot and connect
	Robot_arm ur5 = Robot_arm(ip, velocity, acceleration, dt, lookahead_time, gain, base_pos);
	ur5.connect();

	// Refpoints
	vector<double> ref_point_1 = {-0.09295, -0.42590, 0, 3.14, 0, 0};
	vector<double> ref_point_2 = {0.32133, -0.24458, 0, 2.739, 1.557, 0};
	ur5.set_ref_points(ref_point_1, ref_point_2);

	char in;
	cout << "Want to validate ref points y/n: ";
	cin >> in;
	if (in == 'y') {
		ur5.validate_ref_points();
	}
	

	// Launch GUI, take user input
	
	// Function to calibrate robot
	
	// Main Loop going until finished.
	
}
