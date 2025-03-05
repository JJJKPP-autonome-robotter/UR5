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
    Gripper(string, u_int);

    // Methods
    bool open_gripper();
    bool close_gripper();
    void gripper_send(const string&);
    string gripper_read();

private:
    io_service io;
    serial_port port;

    string port_name;
    u_int baudrate;

};

#endif