#include "../headers/Gripper.hpp"

Gripper::Gripper() {}

Gripper::~Gripper() {
    port.close(); // Close the port
}

// Constructor
Gripper::Gripper(string _port_name, uint32_t _baudrate) : port(io, _port_name) {
    port_name = _port_name;
    baudrate = _baudrate;

    // Set options for the serial port
    port.set_option(serial_port_base::baud_rate(baudrate));
    port.set_option(serial_port_base::character_size(8));
    port.set_option(serial_port_base::parity(serial_port_base::parity::none));
    port.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    port.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
}

// Open the gripper returns true if gripper is open
bool Gripper::open_gripper() {
    string message = "open";
    gripper_send(message); // Send open message to gripper

    bool open = false;
    open = wait_for_target_message("open"); // Waits for response from gripper

    return open;

}

bool Gripper::close_gripper() {
    string message = "close";
    gripper_send(message); // Sends close message to gripper

    bool closed = false;
    closed = wait_for_target_message("closed"); // Waits for response from gripper

    return closed;
}

// Send message to gripper
void Gripper::gripper_send(const string& message) {
    write(port, buffer(message)); // Write message to gripper
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

// Reads until target emssage is received
bool Gripper::wait_for_target_message(string target_message) {
    string input_message;

    // Read until target is received
    while (true) {
        input_message = gripper_read();

        if (input_message == "error") return false;
        if (input_message == target_message) return  true;

    }

}