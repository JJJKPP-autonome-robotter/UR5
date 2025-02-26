# UR5
Code for the UR5 cobot<br>

## Setup
Guid to setting up the robot and needed librarys.<br>

### RTDE Library
Install the RTDE library for C++ By Anders Prier Lindvig.<br>
Library and guid at for installation at [RTDE library.](https://gitlab.com/sdurobotics/ur_rtde)<br>

### OpenCV Library
Install OpenCV Development Package: <br>
    ```bash
    sudo apt install libopencv-dev
    ```
#### Verification

To verify the installation, run<br>
```bash
pkg-config --modversion opencv4
```


### Robot setup
To control the robot over Ethernet. Make sure to install the URCap External Control.<br>
For installation guid see the [URCap installtion.](https://github.com/UniversalRobots/Universal_Robots_ROS_Driver/blob/master/ur_robot_driver/doc/install_urcap_e_series.md)<br>

Create a program on the robot which runs the External Control.<br>
For the External Control, enter the Host IP, local IP for the computer which the remote connection is made with.

### Network setup
On the robot, under External Control, set the Host IP to the local IP of the connected PC.<br>
**To find your local IP run:**<br>
```bash
ifconfig
```
Look for the IP under the Ethernet adapter, should look like 192.168.xxx.xxx<br>
If no IP is assigned for the ethernet adapter, run the following commands to add it.<br>
**Add new local ip:**<br>
```bash
sudo ip addr add "Host IP"/24 dev "Ethernet adapter"
```
**Example:**<br>
```bash
sudo ip addr add 192.168.1.55/24 dev esp2s0
```
**Restart network service:**<br>
```bash
sudo systemctl restart NetworkManager
```
**Check if the Ethernet adpater is UP:**<br>
```bash
ip link show
```
**If Ethernet adapter shows down run:**<br>
```bash
sudo ip link set "Ethernet adapter" up
```
It should now show under ifconfig, and you can now tell the robot what the host IP is.



