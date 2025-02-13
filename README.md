# UR5
Code for the UR5 cobot

## Setup
Guid to setting up the robot and needed librarys

### RTDE Library
Install the RTDE library for C++ By Anders Prier Lindvig
Library and guid at for installation at [RTDE library](https://gitlab.com/sdurobotics/ur_rtde)

### Network setup
On the robot, under external control, set the Host ip to the local ip of your pc.
To find local ip run:
`ifconfig`
Look for the ip under the Ethernet adapter, should look like 192.168.xxx.xxx
If no IP is assigned for the ethernet adapter, run the following to add it.
Add new local ip:
`sudo ip addr add "Host IP"/24 dev "Ethernet adapter"`
Example:
`sudo ip addr add 192.168.1.55/24 dev esp2s0`
Restart network service:
`sudo systemctl restart NetworkManager`
Check if the Ethernet adpater is UP:
`ip link show`
If Ethernet adapter shows down run:
`sudo ip link set "Ethernet adapter" up`
It should now show under ifconfig



