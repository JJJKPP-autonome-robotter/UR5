#include "../headers/Gripper.hpp"

Gripper::Gripper() {}

Gripper::~Gripper() {
    port.close(); // Close the port
}

// Constructor
Gripper::Gripper(string _port_name, u_int _baudrate) : port(io, _port_name) {
    port_name = _port_name;
    baudrate = _baudrate;

    // Set options for the serial port
    port.set_option(serial_port_base::baud_rate(baudrate));
    port.set_option(serial_port_base::character_size(8));
    port.set_option(serial_port_base::parity(serial_port_base::parity::none));
    port.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    port.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
}

// Open the gripper
bool Gripper::open_gripper() {
    string message = "open";
    gripper_send(message);


}

// Send message to gripper
void Gripper::gripper_send(const string& message) {
    write(port, buffer(message));
}

// Read message from gripper
string Gripper::gripper_read() {
    boost::asio::streambuf buffer;
    read_until(port, buffer, '\n'); // Read until newline or time out

    istream input_stream(&buffer);
    string response;
    getline(input_stream, response); // Convert response to string

    return response;
}