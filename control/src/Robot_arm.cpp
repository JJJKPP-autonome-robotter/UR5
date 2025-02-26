#include "../headers/Robot_arm.hpp"

Robot_arm::Robot_arm(){}

Robot_arm::~Robot_arm(){}

// Constructor params IP of robot, velocity, acceleration, frequency, lookahead_time, gain, vecotr of 6 with anlge in rad for each joint
Robot_arm::Robot_arm(string _ip, double _velocity, double _acceleration, double _dt, double _lookahead_time, double _gain, vector<double> _base_pos){
    ip = _ip; // Sets IP
    velocity = _velocity; // Set velocity
    acceleration = _acceleration; // Sets acceleration
    dt = _dt; // Set frequency
    lookahead_time = _lookahead_time; // Sets lookahead_time
    gain = _gain; // Sets gain
    base_pos = _base_pos; // Sets base position vecot of 6 with joint angles in rad
}

// Connect to UR robot arm
void Robot_arm::connect(){
    // Connects to control interface
    cout << "Connecting to robot control interface: ";
    ur_rtde::RTDEControlInterface rtde_control(ip);
    cout << "SUCCES" << endl;

    // Connects to recieve interface
    cout << "Connecting to robot receive interface: ";
    ur_rtde::RTDEReceiveInterface rtde_receive(ip);
    cout << "SUCCES" << endl;

    // Moves robot to base position
    cout << "Moving to base position" << endl;
    rtde_control.moveJ(base_pos, velocity, acceleration);
}

// Getters
double Robot_arm::get_velocity(){
    return velocity;
}

double Robot_arm::get_acceleration(){
    return acceleration;
}

double Robot_arm::get_dt(){
    return dt;
}

double Robot_arm::get_lookahead_time(){
    return lookahead_time;
}

double Robot_arm::get_gain(){
    return gain;
}

vector<double> Robot_arm::get_base_pos(){
    return base_pos;
}


// Setters
void Robot_arm::set_velocity(double v){
    velocity = v;
}

void Robot_arm::set_acceleration(double a){
    acceleration = a;
}

void Robot_arm::set_dt(double _dt){
    dt = _dt;
}

void Robot_arm::set_lookahead_time(double lat){
    lookahead_time = lat;
}

void Robot_arm::set_gain(double g){
    gain = g;
}

void Robot_arm::set_base_pos(vector<double> bp){
    base_pos = bp;
}

