#ifndef GRIPPER_H
#define GRIPPER_H

#pragma once

#include <boost/asio.hpp>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>

using namespace boost::asio;
using namespace std;

class Gripper
{
public:
    ~Gripper();
    Gripper(uint32_t, vector<double>, double); // string Port name u_int baudrate

    // Methods
    bool open(); // Opens gripper
    bool close(); // Closes gripper
    bool pickup(); // Check if gripper has successfull pickup
    void send(const string&); // Sends message
    string read(); // Reads unkown length response
    bool waitForTargetMessage(string); // Waits for target message
    string findPort(); // Finds the port the Pico is connected to

    vector<double> getOffset();
    double getPayload();

private:
    io_service io;
    serial_port port;

    string portName;
    uint32_t baudrate;

    vector<double> offset;
    double payload;

};

#endif