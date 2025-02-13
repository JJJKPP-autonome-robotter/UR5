# UR5
Code for the UR5 cobot<br>

## Setup
Guid to setting up the robot and needed librarys<br>

### RTDE Library
Install the RTDE library for C++ By Anders Prier Lindvig<br>
Library and guid at for installation at [RTDE library](https://gitlab.com/sdurobotics/ur_rtde)<br>

### Network setup
On the robot, under external control, set the Host ip to the local ip of your pc.<br>
To find local ip run:<br>
`ifconfig`
Look for the ip under the Ethernet adapter, should look like 192.168.xxx.xxx<br>
If no IP is assigned for the ethernet adapter, run the following to add it.<br>
Add new local ip:<br>
`sudo ip addr add "Host IP"/24 dev "Ethernet adapter"`
Example:<br>
`sudo ip addr add 192.168.1.55/24 dev esp2s0`
Restart network service:<br>
`sudo systemctl restart NetworkManager`
Check if the Ethernet adpater is UP:<br>
`ip link show`
If Ethernet adapter shows down run:<br>
`sudo ip link set "Ethernet adapter" up`
It should now show under ifconfig



