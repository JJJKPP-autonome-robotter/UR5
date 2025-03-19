#include "../headers/Robot_arm.hpp"
#include "../headers/Gripper.hpp"

Robot_arm::Robot_arm() {}

Robot_arm::~Robot_arm() {
    cout << "Stopping control script, and disconnecting robot" << endl;

    // Stop control script
    rtde_control->stopScript();

    // Disconnect rom robot
    rtde_control->disconnect();
    rtde_receive->disconnect();
}

// Constructor params IP of robot, velocity, acceleration, frequency, lookahead_time, gain, vecotr of 6 with anlge in rad for each joint
Robot_arm::Robot_arm(string _ip, double _velocity, double _acceleration, double _dt, double _lookahead_time, double _gain, vector<double> _base_pos, Gripper* _gripper){
    ip = _ip; // Sets IP
    velocity = _velocity; // Set velocity
    acceleration = _acceleration; // Sets acceleration
    dt = _dt; // Set frequency
    lookahead_time = _lookahead_time; // Sets lookahead_time
    gain = _gain; // Sets gain
    base_pos = _base_pos; // Sets base position vecot of 6 with joint angles in rad
    gripper = _gripper; // Sets gripper class

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

    // Set TCP offset for griper
    vector<double> offset = gripper->get_offset();
    rtde_control->setTcp(offset);

    // Moves robot to base position
    cout << "Moving to base position" << endl;
    rtde_control->moveL(base_pos);

    // Opens gripper
    gripper->open();
}

// validate ref points
void Robot_arm::validate_ref_points() {
    double _velocity = 0.05;

    // Open gripper
    gripper->close();

    // Get hover over ref point 1
    vector<double> hover_ref = ref_point_1;
    hover_ref[2] += 0.3; // Add 10cm to z-axis

    // Hover over ref point 1
    rtde_control->moveL(hover_ref);
    rtde_control->moveL(ref_point_1, _velocity, acceleration); // Go down and touch

    // Wait for confirm
    // If not update ref coordinates
    bool ref1_update = confirm_point(ref_point_1);
    if (ref1_update) {
        hover_ref = ref_point_1;
        hover_ref[2] += 0.3;
    }
    rtde_control->moveL(hover_ref, _velocity, acceleration); // Hover over ref point 1


    // Get hover over ref point 2
    hover_ref = ref_point_2;
    hover_ref[2] += 0.3; // Add 10cm to z-axis

    // Hover over ref point 2
    rtde_control->moveL(hover_ref);    
    rtde_control->moveL(ref_point_2, _velocity, acceleration); // Go down and touch

    // Wait for confitm
    // If not update ref coordinates
    bool ref2_update = confirm_point(ref_point_2);

    // Hover over ref point 2
    if (ref2_update) {
        hover_ref = ref_point_2;
        hover_ref[2] += 0.3;
    }
    rtde_control->moveL(hover_ref, _velocity, acceleration); 

    // Go to base pos
    rtde_control->moveL(base_pos);
    gripper->open();

    // If ref points have been updated validate again
    if (ref1_update || ref2_update) {
        cout << "Validating new ref points" << endl;
        validate_ref_points();
    }
}

// Validate drop points
void Robot_arm::validate_drop_points() {
    bool any_updated = false;

    // Open gripper
    gripper->close();

    // Loop thrue all drop points
    for (int i = 0; i < drop_points.size(); i++) {
        // Get color and coordinate for drop
        auto drop_point = next(drop_points.begin(), i);
        string color = drop_point->first;
        vector<double> point = drop_point->second;

        // Move to drop point
        rtde_control->moveL(point);

        cout << "Drop point for " << color << endl;

        // Confirm point
        if (confirm_point(point)) {
            drop_points[color] = point;
            any_updated = true;
        } 
    }

    // If any of the drop points have been updated validate again
    if (any_updated) {
        cout << "validating new drop points" << endl;
        validate_drop_points();
    }

    // Return to base pos
    rtde_control->moveL(base_pos);
    gripper->open();
}

// Confirm point by user
bool Robot_arm::confirm_point(vector<double>& ref_point) {
    // Wait for user input
    char in;
    cout << "Is tool on point y/n: ";
    cin >> in;
    if (in == 'n') {
        // Enter freedrive mode
        rtde_control->teachMode();
        cout << "Move tool to actual ref point.\n";

        // Wait for user input
        cout << "Is tool on point y/n: ";
        cin >> in;
        if (in == 'y') {
            // Exit freedrive mode
            rtde_control->endTeachMode();
            
            vector<double> new_ref_point = rtde_receive->getTargetTCPPose();
            ref_point = new_ref_point;

            // Print out new ref point
            cout << "New point is: ";
            for (auto i: new_ref_point) cout << i << ", ";
            cout << endl;

            return true;
        }
    }

    return false;
}

void Robot_arm::pick_up(string color, vector<double> point) {
    double _velocity = 0.1; // Pick up velocity

    point.insert(point.end(), {0.1, 3.14, 0, 0}); // Add table height and tool rotation

    // Make hover point, to avoid collision
    vector<double> hover_point = point;
    hover_point[2] += 0.2;

    // Hover over point
    rtde_control->moveL(hover_point);

    // Open gripper
    gripper->open();

    // Lower to point
    rtde_control->moveL(point, _velocity);

    // Close gripper
    gripper->close();

    // Hover over point
    rtde_control->moveL(hover_point, _velocity);

    // Go to drop_point
    rtde_control->moveL(drop_points[color]);

    // Open gripper
    gripper->open();

    // Go to base position
    rtde_control->moveL(base_pos);

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

void Robot_arm::set_drop_points(unordered_map<string, vector<double>> _drop_points) {
    drop_points = _drop_points;
}
