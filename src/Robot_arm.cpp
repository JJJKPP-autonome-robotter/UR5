#include "../headers/Robot_arm.hpp"
#include "../headers/Gripper.hpp"

RobotArm::RobotArm() {}

RobotArm::~RobotArm() {
    cout << "Stopping control script, and disconnecting robot" << endl;

    // Stop control script
    rtdeControl->stopScript();

    // Disconnect rom robot
    rtdeControl->disconnect();
    rtdeReceive->disconnect();
}

// Constructor params IP of robot, velocity, acceleration, frequency, lookahead_time, gain, vecotr of 6 with anlge in rad for each joint
RobotArm::RobotArm(string _ip, double _velocity, double _acceleration, double _dt, double _lookaheadTime, double _gain, vector<double> _basePos, Gripper* _gripper){
    ip = _ip; // Sets IP
    velocity = _velocity; // Set velocity
    acceleration = _acceleration; // Sets acceleration
    dt = _dt; // Set frequency
    lookaheadTime = _lookaheadTime; // Sets lookahead_time
    gain = _gain; // Sets gain
    basePos = _basePos; // Sets base position vecot of 6 with joint angles in rad
    gripper = _gripper; // Sets gripper class

}

// Connect to UR robot arm
void RobotArm::connect() {
    // Connects to control interface
    cout << "Connecting to robot control interface: " << endl;
    rtdeControl = new ur_rtde::RTDEControlInterface(ip);
    cout << "SUCCES" << endl;

    // Connects to recieve interface
    cout << "Connecting to robot receive interface: " << endl;
    rtdeReceive = new ur_rtde::RTDEReceiveInterface(ip);
    cout << "SUCCES" << endl;

    // Set TCP offset and payload for griper
    vector<double> offset = gripper->getOffset();
    double payload = gripper->getPayload();
    rtdeControl->setTcp(offset);
    rtdeControl->setPayload(payload);

    // Moves robot to base position
    cout << "Moving to base position" << endl;
    rtdeControl->moveL(basePos);

    // Opens gripper
    gripper->open();
}

// validate ref points
void RobotArm::validateRefPoints() {
    double _velocity = 0.05;

    // Open gripper
    gripper->close();

    // Get hover over ref point 1
    vector<double> hoverRef = refPoint1;
    hoverRef[2] += 0.1; // Add 10cm to z-axis

    // Hover over ref point 1
    rtdeControl->moveL(hoverRef);
    rtdeControl->moveL(refPoint1, _velocity, acceleration); // Go down and touch

    // Wait for confirm
    // If not update ref coordinates
    bool ref1Update = confirmPoint(refPoint1);
    if (ref1Update) {
        hoverRef[2] += 0.1;
    }
    rtdeControl->moveL(hoverRef, _velocity, acceleration); // Hover over ref point 1


    // Get hover over ref point 2
    hoverRef = refPoint2;
    hoverRef[2] += 0.1; // Add 10cm to z-axis

    // Hover over ref point 2
    rtdeControl->moveL(hoverRef);    
    rtdeControl->moveL(refPoint2, _velocity, acceleration); // Go down and touch

    // Wait for confitm
    // If not update ref coordinates
    bool ref2Update = confirmPoint(refPoint2);

    // Hover over ref point 2
    if (ref2Update) {
        hoverRef = refPoint2;
        hoverRef[2] += 0.1;
    }
    rtdeControl->moveL(hoverRef, _velocity, acceleration);

    // Get hover over ref point 3
    hoverRef = refPoint3;
    hoverRef[2] += 0.1; // Add 10cm to z-axis

    // Hover over ref point 3
    rtdeControl->moveL(hoverRef);    
    rtdeControl->moveL(refPoint3, _velocity, acceleration); // Go down and touch

    // Wait for confitm
    // If not update ref coordinates
    bool ref3Update = confirmPoint(refPoint3);

    // Hover over ref point 2
    if (ref3Update) {
        hoverRef = refPoint3;
        hoverRef[2] += 0.1;
    }
    rtdeControl->moveL(hoverRef, _velocity, acceleration); 

    // Go to base pos
    rtdeControl->moveL(basePos);
    gripper->open();

    // If ref points have been updated validate again
    if (ref1Update || ref2Update) {
        cout << "Validating new ref points" << endl;
        validateRefPoints();
    }
}

// Validate drop points
void RobotArm::validateDropPoints() {
    bool anyUpdated = false;

    // Open gripper
    gripper->close();

    // Loop thrue all drop points
    for (int i = 0; i < dropPoints.size(); i++) {
        // Get color and coordinate for drop
        auto dropPoint = next(dropPoints.begin(), i);
        string color = dropPoint->first;
        vector<double> point = dropPoint->second;

        // Move to drop point
        rtdeControl->moveL(point);

        cout << "Drop point for " << color << endl;

        // Confirm point
        if (confirmPoint(point)) {
            dropPoints[color] = point;
            anyUpdated = true;
        } 
    }

    // If any of the drop points have been updated validate again
    if (anyUpdated) {
        cout << "validating new drop points" << endl;
        validateDropPoints();
    }

    // Return to base pos
    rtdeControl->moveL(basePos);
    gripper->open();
}

// Confirm point by user
bool RobotArm::confirmPoint(vector<double>& refPoint) {
    // Wait for user input
    char in;
    cout << "Is tool on point y/n: ";
    cin >> in;
    if (in == 'n') {
        // Enter freedrive mode
        rtdeControl->teachMode();
        cout << "Move tool to actual ref point.\n";

        // Wait for user input
        cout << "Is tool on point y/n: ";
        cin >> in;
        if (in == 'y') {
            // Exit freedrive mode
            rtdeControl->endTeachMode();
            
            vector<double> newRefPoint = rtdeReceive->getTargetTCPPose();
            refPoint = newRefPoint;

            // Print out new ref point
            cout << "New point is: ";
            for (auto i: newRefPoint) cout << i << ", ";
            cout << endl;

            return true;
        }
    }

    return false;
}

void RobotArm::pickUp(string color, vector<double> point) {
    double _velocity = 0.1; // Pick up velocity

    point.insert(point.end(), {-0.191, 3.14, 0, 0}); // Add table height and tool rotation

    // Make hover point, to avoid collision
    vector<double> hoverPoint = point;
    hoverPoint[2] += 0.2;

    // Hover over point
    rtdeControl->moveL(hoverPoint);

    // Open gripper
    gripper->open();

    // Lower to point
    rtdeControl->moveL(point, _velocity);

    // Close gripper
    gripper->close();

    // Hover over point
    rtdeControl->moveL(hoverPoint, _velocity);

    // Go to drop_point
    rtdeControl->moveL(dropPoints[color]);

    // Open gripper
    gripper->open();

    // Go to base position
    rtdeControl->moveL(basePos);

}

// Getters
// Gets double velocity and returns double velocity
double RobotArm::getVelocity(){
    return velocity;
}

// Gets double acceleration and returns double accelertation
double RobotArm::getAcceleration(){
    return acceleration;
}

// Gets double dt and returns double dt
double RobotArm::getDt(){
    return dt;
}

// Gets double lookahead_time and returns double lookahead_time
double RobotArm::getLookaheadTime(){
    return lookaheadTime;
}

// Gets double gain and returns double gain
double RobotArm::getGain(){
    return gain;
}

// Gets vector (6 elements: joint angles in rads) double base_pos and returns vector (6 elements: joint angles in rads) double base_pos 
vector<double> RobotArm::getBasePos(){
    return basePos;
}

// Gets vector (6 element: joint angles in rads) double and returns vector (6 element: joint angles in rads) double
vector<double> RobotArm::getRefPoint1(){
    return refPoint1;
}

// Gets vector (6 element: joint angles in rads) double and returns vector (6 element: joint angles in rads) double
vector<double> RobotArm::getRefPoint2(){
    return refPoint2;
}

// Gets vector (6 element: joint angles in rads) double and returns vector (6 element: joint angles in rads) double
vector<double> RobotArm::getRefPoint3(){
    return refPoint3;
}

unordered_map<string, vector<double>> RobotArm::getDropPoints() {
    return dropPoints;
}

// Setters
// Sets velocity in Robot_arm to the variable "v"
void RobotArm::setVelocity(double v){
    velocity = v;
}

// Sets acceleration in Robot_arm to the variable "a"
void RobotArm::setAcceleration(double a){
    acceleration = a;
}

// Sets dt in Robot_arm to the variable "_dt"
void RobotArm::setDt(double _dt){
    dt = _dt;
}

// Sets lookahead_time in Robot_arm to the variable "lat"
void RobotArm::setLookaheadTime(double lat){
    lookaheadTime = lat;
}

// Sets gain in Robot_arm to the variable "g"
void RobotArm::setGain(double g){
    gain = g;
}

// Sets base_pos in Robot_arm to the variable "bp"
void RobotArm::setBasePos(vector<double> bp){
    basePos = bp;
}

// Sets ref_point_1 to variable ref1, and ref_point_2 to ref2
void RobotArm::setRefPoints(vector<double> ref1, vector<double> ref2, vector<double> ref3){
    refPoint1 = ref1;
    refPoint2 = ref2;
    refPoint3 = ref3;
}

void RobotArm::setDropPoints(unordered_map<string, vector<double>> _dropPoints) {
    dropPoints = _dropPoints;
}
