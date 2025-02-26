#ifndef ROBOT_ARM_H
#define ROBOT_ARM_H

#pragma once

#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <array>

using namespace std;

class Robot_arm
{
public:
    Robot_arm();
    ~Robot_arm();
    // Constructor param IP, Velocity, Accelerations, Frequency, Lookahead_time, gain, vector 6 joint agnles in rad
    Robot_arm(string, double, double, double, double, double, vector<double>);

    // Methods
    void connect();// Connects to Robot arm
    void validate_ref_point(); // Validate refpoints, vector3 coordiantes to points

    // Getters
    double get_velocity();
    double get_acceleration();
    double get_dt();
    double get_lookahead_time();
    double get_gain();
    vector<double> get_base_pos();
    vector<double> get_ref_point_1();
    vector<double> get_ref_point_2();

    // Setters
    void set_velocity(double);
    void set_acceleration(double);
    void set_dt(double);
    void set_lookahead_time(double);
    void set_gain(double);
    void set_base_pos(vector<double>);
    void set_ref_points(vector<double>, vector<double>);

private:
    ur_rtde::RTDEControlInterface* rtde_control = nullptr;
    ur_rtde::RTDEReceiveInterface* rtde_receive = nullptr;
    string ip;
    double velocity;
    double acceleration;
    double dt;
    double lookahead_time;
    double gain;
    vector<double> base_pos;
    vector<double> ref_point_1;
    vector<double> ref_point_2;

};

#endif