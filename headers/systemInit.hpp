#ifndef SYSTEMINIT_HPP
#define SYSTEMINIT_HPP

#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../headers/CaptureImage.hpp"
#include "../headers/Data_saver.hpp"
#include "../headers/Gripper.hpp"
#include "../headers/PixelToRobot.hpp"
#include "../headers/ProcessImage.hpp"
#include "../headers/Robot_arm.hpp"
#include "../headers/DataLogger.hpp"

extern ConfigFile cfg;
extern RobotArm *ur5;
extern Gripper *gripper;
extern CaptureImage camera;
extern PixelToRobot *pixelToRobot;
extern unordered_map<string, vector<double>> dropPoints;

void initializeRobot();
void calibrateSystem();
pair<Point, string> captureAndProcess(const vector<string> &selectedColors);
DataLogger initDataLogger();

#endif
