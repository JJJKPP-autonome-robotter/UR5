#include "../headers/Robot_arm.hpp"

Robot_arm::Robot_arm() {}

Robot_arm::~Robot_arm() {}

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
void Robot_arm::connect() {
    // Connects to control interface
    cout << "Connecting to robot control interface: " << endl;
    rtde_control = new ur_rtde::RTDEControlInterface(ip);
    cout << "SUCCES" << endl;

    // Connects to recieve interface
    cout << "Connecting to robot receive interface: " << endl;
    rtde_receive = new ur_rtde::RTDEReceiveInterface(ip);
    cout << "SUCCES" << endl;

    // Moves robot to base position
    cout << "Moving to base position" << endl;
    rtde_control->moveJ(base_pos, velocity, acceleration);
}

// Place reference points
void Robot_arm::validate_ref_point() {
    // Go to ref point one
    vector<double> hover_ref = ref_point_1;
    hover_ref[3] += 0.1;
    rtde_control->moveL(hover_ref); // Hover over ref point 1
    rtde_control->moveL(ref_point_1); // Go down and touch

    // Wait for confirm
    // If not update ref coordinates
    int confirm;
    cin >> confirm;
    if (confirm == 0) {
        // Update ref point
    }

    rtde_control->moveL(hover_ref); // Hover over ref point 1


    // Go to ref point two
    hover_ref = ref_point_2;
    hover_ref[3] += 0.1;
    rtde_control->moveL(hover_ref); // Hover over ref point 2
    rtde_control->moveL(ref_point_1); // Go down and touch

    // Wait for confitm
    // If not update ref coordinates
    cin >> confirm;
    if (confirm == 0) {
        // Update ref point
    }

    rtde_control->moveL(hover_ref); // Hover over ref point 2
}

// Getters
// Gets double velocity and returns double velocity
double Robot_arm::get_velocity(){
    return velocity;
}

// Gets double acceleration and returns double accelertation
double Robot_arm::get_acceleration(){
    return acceleration;
}

// Gets double dt and returns double dt
double Robot_arm::get_dt(){
    return dt;
}

// Gets double lookahead_time and returns double lookahead_time
double Robot_arm::get_lookahead_time(){
    return lookahead_time;
}

// Gets double gain and returns double gain
double Robot_arm::get_gain(){
    return gain;
}

// Gets vector (6 elements: joint angles in rads) double base_pos and returns vector (6 elements: joint angles in rads) double base_pos 
vector<double> Robot_arm::get_base_pos(){
    return base_pos;
}

// Gets vector (6 element: joint angles in rads) double and returns vector (6 element: joint angles in rads) double
vector<double> Robot_arm::get_ref_point_1(){
    return ref_point_1;
}

// Gets vector (6 element: joint angles in rads) double and returns vector (6 element: joint angles in rads) double
vector<double> Robot_arm::get_ref_point_2(){
    return ref_point_2;
}


// Setters
// Sets velocity in Robot_arm to the variable "v"
void Robot_arm::set_velocity(double v){
    velocity = v;
}

// Sets acceleration in Robot_arm to the variable "a"
void Robot_arm::set_acceleration(double a){
    acceleration = a;
}

// Sets dt in Robot_arm to the variable "_dt"
void Robot_arm::set_dt(double _dt){
    dt = _dt;
}

// Sets lookahead_time in Robot_arm to the variable "lat"
void Robot_arm::set_lookahead_time(double lat){
    lookahead_time = lat;
}

// Sets gain in Robot_arm to the variable "g"
void Robot_arm::set_gain(double g){
    gain = g;
}

// Sets base_pos in Robot_arm to the variable "bp"
void Robot_arm::set_base_pos(vector<double> bp){
    base_pos = bp;
}

// Sets ref_point_1 to variable ref1, and ref_point_2 to ref2
void Robot_arm::set_ref_points(vector<double> ref1, vector<double> ref2){
    ref_point_1 = ref1;
    ref_point_2 = ref2;
}
