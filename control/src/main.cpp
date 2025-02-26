#include <../headers/Robot_arm.hpp>

#include <iostream>
#include <string>
#include <array>
#include <vector>

using namespace std;

int main() {
	string ip = "192.168.1.54";
	double velocity = 0.25; // Standart velocity
    double acceleration = 0.25; // Standart acceleration
    double dt = 1.0/500; // Standart update rate
    double lookahead_time = 0.1; // Standart lookahead time
    double gain = 300; // Standard gain
    vector<double> base_pos = {-0.98, -2.15, -0.36, 0.05, 3.14, 0.71}; // base position for program

	// Init robot and connect
	Robot_arm ur5 = Robot_arm(ip, velocity, acceleration, dt, lookahead_time, gain, base_pos);
	ur5.connect();

	// Refpoints
	vector<double> ref_point_1 = {5, 5, 5, 0, 0, 0};
	vector<double> ref_point_2 = {5, 10, 5, 0, 0, 0};
	ur5.set_ref_points(ref_point_1, ref_point_2);

	// Launch GUI, take user input
	
	// Function to calibrate robot
	
	// Main Loop going until finished.
	
}
