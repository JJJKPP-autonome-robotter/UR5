#ifndef GRIBBER_H
#define GRIBBER_H

#pragma once

#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;
using namespace std;

class Gribber
{
public:
    Gribber();
    ~Gribber();
    Gribber(string, u_int);

private:
    io_service io;
    serial_port port;

    string port_name;
    u_int baudrate;

};

#endif