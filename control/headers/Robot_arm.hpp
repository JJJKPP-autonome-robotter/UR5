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