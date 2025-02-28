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
    void validate_ref_points(); // Validate refpoints

    // Getters
    double get_velocity(); // Velocity double
    double get_acceleration(); // Acceleration double
    double get_dt(); // Frequency double
    double get_lookahead_time(); // Lookahead_time double
    double get_gain(); // Gain double
    vector<double> get_base_pos(); // Base pos vector<double> {x, y, z, rx, ry, rz}
    vector<double> get_ref_point_1(); // Ref point 1 vector<double> {x, y, z, rx, ry, rz}
    vector<double> get_ref_point_2(); // Ref point 2 vector<double> {x, y, z, rx, ry, rz}

    // Setters
    void set_velocity(double); // Velocity double
    void set_acceleration(double); // Acceleration double
    void set_dt(double); // Frequency double
    void set_lookahead_time(double); // Lookahead_time double
    void set_gain(double); // Gain double
    void set_base_pos(vector<double>); // Base pos vector<double> {x, y, z, rx, ry, rz}
    void set_ref_points(vector<double>, vector<double>); // Ref points vector<double> {x, y, z, rx, ry, rz}

private:
    // Robot specific objects and variables
    ur_rtde::RTDEControlInterface* rtde_control = nullptr;
    ur_rtde::RTDEReceiveInterface* rtde_receive = nullptr;
    chrono::steady_clock::time_point t_start;
    string ip;

    // Movement variables
    double velocity;
    double acceleration;
    double dt;
    double lookahead_time;
    double gain;

    // Stadnart positions
    vector<double> base_pos;
    vector<double> ref_point_1;
    vector<double> ref_point_2;

    // Privately used functions
    void confirm_point(vector<double>&);

};

#endif