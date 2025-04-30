#ifndef ROBOT_ARM_H
#define ROBOT_ARM_H

#pragma once

#include "Gripper.hpp"
#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <array>

using namespace std;

class RobotArm
{
public:
    RobotArm();
    ~RobotArm();
    // Constructor param IP, Velocity, Accelerations, Frequency, Lookahead_time, gain, vector 6 joint agnles in rad
    RobotArm(string, double, double, double, double, double, vector<double>, vector<double>, Gripper*);

    // Methods
    void connect();// Connects to Robot arm
    void validateRefPoints(); // Validate refpoints
    void validateDropPoints(); // Validate drop points
    bool pickUp(string, vector<double>); // Pick up color from point vector<double> {x, y}

    // Getters
    double getVelocity(); // Velocity double
    double getAcceleration(); // Acceleration double
    double getDt(); // Frequency double
    double getLookaheadTime(); // Lookahead_time double
    double getGain(); // Gain double
    vector<double> getBasePos(); // Base pos vector<double> {x, y, z, rx, ry, rz}
    vector<double> getRefPoint1(); // Ref point 1 vector<double> {x, y, z, rx, ry, rz}
    vector<double> getRefPoint2(); // Ref point 2 vector<double> {x, y, z, rx, ry, rz}
    vector<double> getRefPoint3(); // Ref point 3 vector<double> {x, y, z, rx, ry, rz}
    unordered_map<string, vector<double>> getDropPoints(); // Drop points unordered map color : vector<double> {x, y, z, rx, ry, rz}

    // Setters
    void setVelocity(double); // Velocity double
    void setAcceleration(double); // Acceleration double
    void setDt(double); // Frequency double
    void setLookaheadTime(double); // Lookahead_time double
    void setGain(double); // Gain double
    void setBasePos(vector<double>); // Base pos vector<double> {x, y, z, rx, ry, rz}
    void setRefPoints(vector<double>, vector<double>, vector<double>); // Ref points vector<double> {x, y, z, rx, ry, rz}
    void setDropPoints(unordered_map<string, vector<double>>); // Map drop points to color and vector<double> {x, y, z, rx, ry, rz}

private:
    // Robot specific objects and variables
    ur_rtde::RTDEControlInterface* rtdeControl = nullptr;
    ur_rtde::RTDEReceiveInterface* rtdeReceive = nullptr;
    string ip;

    // Gripper
    Gripper* gripper;
    bool pickStatus;

    // Movement variables
    double velocity;
    double acceleration;
    double dt;
    double lookaheadTime;
    double gain;

    // Stadnart positions
    vector<double> basePos;
    vector<double> refPoint1;
    vector<double> refPoint2;
    vector<double> refPoint3;
    vector<double> pickupHeight;
    unordered_map<string, vector<double>> dropPoints;

    // Privat functions
    bool confirmPoint(vector<double>&);

};

#endif