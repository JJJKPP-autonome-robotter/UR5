#include <iostream>
#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>
#include <thread>
#include <chrono>
#include <string>

using namespace ur_rtde;
using namespace std::chrono;


int main(int argc, char* argv[]){
	std::cout << "Connecting to control" << std::endl;
	RTDEControlInterface rtde_control("192.168.0.133");
	std::cout << "Connecting to recieve" << std::endl;
	RTDEReceiveInterface rtde_receive("192.168.0.133");
	std::cout << "ALL Connections established!!" << std::endl;

	double velocity = 0.5;
	double acceleration = 0.5;
	double dt = 1.0/500;
	double lookahead_time = 0.1;
	double gain = 300;
	std::vector<double> joint_q = {-0.98, -2.15, -0.36, 0.05, 3.14, 0.71};

	rtde_control.moveJ(joint_q);

	for (unsigned int i=0; i<1500; i++){
		steady_clock::time_point t_start = rtde_control.initPeriod();
		rtde_control.servoJ(joint_q, velocity, acceleration, dt, lookahead_time, gain);

		std::vector<double> current_joint_positions = rtde_receive.getActualQ();

		std::cout << "Current joint positions at step: " << i + 1 << std::endl;

		for (const auto& joint : current_joint_positions){
			std::cout << joint << std::endl;
		}

		joint_q[0] += 0.001;
		joint_q[1] += 0.001;
		rtde_control.waitPeriod(t_start);
	}

	rtde_control.servoStop();
	rtde_control.stopScript();

	return 0;
}
