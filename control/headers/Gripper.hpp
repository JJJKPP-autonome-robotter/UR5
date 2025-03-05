#ifndef GRIPPER_H
#define GRIPPER_H

#pragma once

#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;
using namespace std;

class Gripper
{
public:
    Gripper();
    ~Gripper();
    Gripper(string, uint32_t); // string Port name u_int baudrate

    // Methods
    bool open_gripper(); // Opens gripper
    bool close_gripper(); // Closes gripper
    void gripper_send(const string&); // Sends message
    string gripper_read(); // Reads unkown length response
    bool wait_for_target_message(string); // Waits for target message

private:
    io_service io;
    serial_port port;

    string port_name;
    uint32_t baudrate;

};

#endif