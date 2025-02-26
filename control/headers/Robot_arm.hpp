#ifndef ROBOT_ARM_H
#define ROBOT_ARM_H

#pragma once

#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>
#include <thread>
#include <chrono>
#include <string>

using namespace std;

class Robot_arm
{
public:
    Robot_arm();
    ~Robot_arm();
    Robot_arm(string, double, double, double, double, double, vector<double>);

    void connect();
    void place_ref_point();

    // Getters
    double get_velocity();
    double get_acceleration();
    double get_dt();
    double get_lookahead_time();
    double get_gain();
    vector<double> get_base_pos();

    // Setters
    void set_velocity(double);
    void set_acceleration(double);
    void set_dt(double);
    void set_lookahead_time(double);
    void set_gain(double);
    void set_base_pos(vector<double>)

private:
    string ip;
    double velocity;
    double acceleration;
    double dt;
    double lookahead_time;
    double gain;
    vector<double> base_pos;

};

#endif