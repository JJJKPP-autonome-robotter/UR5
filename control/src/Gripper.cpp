#include "../headers/Gripper.hpp"


Gripper::~Gripper() {
    port.close(); // Close the port
}

// Constructor
Gripper::Gripper(uint32_t _baudrate, vector<double> _offset, double _payload) : port(io) {
    portName = findPort(); // Get device addres
    baudrate = _baudrate;
    offset = _offset;
    payload = _payload;

    // Check if device was found
    if (portName.empty()) throw runtime_error("No valid device found");

    // Open port
    port.open(portName);

    // Set options for the serial port
    port.set_option(serial_port_base::baud_rate(baudrate));
    port.set_option(serial_port_base::character_size(8));
    port.set_option(serial_port_base::parity(serial_port_base::parity::none));
    port.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    port.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
}

// Open the gripper returns true if gripper is open
bool Gripper::open() {
    string message = "o";
    send(message); // Send open message to gripper

    bool open = false;
    open = waitForTargetMessage("open"); // Waits for response from gripper

    if (open) cout << "OPEN" << endl;

    return open;

}

bool Gripper::close() {
    string message = "c";
    send(message); // Sends close message to gripper

    bool closed = false;
    closed = waitForTargetMessage("closed"); // Waits for response from gripper

    if (closed) cout << "CLOSED" << endl;

    return closed;
}

// Send message to gripper
void Gripper::send(const string& message) {
    write(port, buffer(message)); // Write message to gripper
}

// Read message from gripper
string Gripper::read() {
    boost::asio::streambuf buffer;
    read_until(port, buffer, '\n'); // Read until newline or time out

    istream inputStream(&buffer);
    string response;
    getline(inputStream, response); // Convert response to string

    response.erase(remove_if(response.begin(), response.end(), [](char c) {
        return !isalpha(c);
    }), response.end());

    return response;
}

// Reads until target emssage is received
bool Gripper::waitForTargetMessage(string target_message) {
    string inputMessage;

    // Read until target is received
    while (true) {
        inputMessage = read(); // Read pico response

        if (inputMessage == "error") return false;
        if (inputMessage == target_message) return  true;

    }

    return false;
}

string Gripper::findPort() {
    cout << "Scanning for Raspberry Pi Pico..." << endl;

    // Load all usb devices
    for (const auto& entry : std::filesystem::directory_iterator("/dev/")) {
        std::string path = entry.path().string(); // Get path of device

        // Look for ttyACM or ttyUSB devices
        if (path.find("ttyACM") != std::string::npos || path.find("ttyUSB") != std::string::npos) {  

            // Check USB device info using lsusb
            std::stringstream cmd;
            cmd << "lsusb -d 2e8a:000a | grep -q 'Raspberry Pi Pico'";

            int result = system(cmd.str().c_str());

            // If Raspberry Pi Pico found return path
            if (result == 0) {
                std::cout << "Found Raspberry Pi Pico at: " << path << std::endl;
                return path;
            }
        }
    }

    // If no path found return empty
    cout << "No raspberry Pi Pico found" << endl;
    return "";
}

vector<double> Gripper::getOffset() {
    return offset;
}

double Gripper::getPayload() {
    return payload;
}