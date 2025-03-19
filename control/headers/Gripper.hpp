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
    Gripper(uint32_t, vector<double>); // string Port name u_int baudrate

    // Methods
    bool open(); // Opens gripper
    bool close(); // Closes gripper
    void send(const string&); // Sends message
    string read(); // Reads unkown length response
    bool wait_for_target_message(string); // Waits for target message
    string find_port(); // Finds the port the Pico is connected to

    vector<double> get_offset();

private:
    io_service io;
    serial_port port;

    string port_name;
    uint32_t baudrate;

    vector<double> offset;

};

#endif