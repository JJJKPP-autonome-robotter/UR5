#include "../headers/Robot_arm.hpp"

Robot_arm::Robot_arm(){}

Robot_arm::~Robot_arm(){}

Robot_arm::Robot_arm(string _ip, double _velocity, double _acceleration, double _dt, double _lookahead_time, double _gain, vector<double> _base_pos){
    ip = _ip;
    velocity = _velocity;
    acceleration = _acceleration;
    dt = _dt;
    lookahead_time = _lookahead_time;
    gain = _gain;
    base_pos = _base_pos;
}

void Robot_arm::connect(){
    cout << "Connecting to robot control interface: ";
    ur_rtde::RTDEControlInterface rtde_control(ip);
    cout << "SUCCES" << endl;

    cout << "Connecting to robot receive interface: ";
    ur_rtde::RTDEReceiveInterface rtde_receive(ip);
    cout << "SUCCES" << endl;

    cout << "Moving to base position" << endl;
    rtde_control.moveJ(base_pos, velocity, acceleration);
}