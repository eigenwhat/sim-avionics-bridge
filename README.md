# sim-avionics-bridge

## Avionics barebone example bridge for MSFS and X-Plane using ADBP and NMEA over UDP.

This is a very basic example of of two bridge plugins to send/broadcast position, heading/course and speed data from MSFS 2020 and X-Plane flight simulators onto the network using both Teledyne ADIF/ADBP and NMEA0183 protocols over UDP. 

It simply connects to the simulator and broadcasts data at a frequency of 10Hz (configurable in source) to EFB apps listening on the given IP address.

- ADIF/ADBP messages are sent to port 50000. 
- NMEA 0183 messages (RMC, GLL and HDG) are sent to port 10110, one per line. 

## Run 

- Clone the repository

### For MSFS 2020: 

1. Open a command Launch simulator, load plane / airport, etc 
2. Open a command line window: 
3. Run: FSAvionicsBridge.exe X.X.X.X

where X.X.X.X is the IP address of the destination device consuming data on port 50000 or 10110. Alternatively you can use the network's breadcast IP address to reach all devices listening, for example: 

For IP address 192.168.0/24 the IP address 192.168.0.255 is the broadcast address.
For IP address 10.0.0.0/24 the IP address 10.0.0.255 is the broadcast address.
For IP address 10.0.0.0/8 the IP address 10.255.255.255 is the broadcast address.

Important: You need to have SimConnect.dll on the same directory of the exe file. 

### For X-Plane: 

1. Copy the corresponding plugin file to the Resouces/plugins folder of X-Plane
 
   When running X-Plane on Windows copy XPAvionicsBridgeWin.xpl

   When running X-Plane on Mac copy XPAvionicsBridgeMac.xpl

3. Configure the destination IP address (as for the MSFS plugin) by editing the file Resouces/plugins/XPAvionicsBridge.cfg and adding the IP address only on it without a new line.

   e.g. 192.168.1.255

4. Launch X-Plane. The plugin should appear in the list of plugins loaded. Some messages are logged to Log.txt
   
## Build

A Makefile is provided for quick build on macOS. Do this from terminal / command line and not from X-Code. 

1. Install x86_64-w64-mingw32 from ports (to build Windows binaries on Mac)

2. Type:
   
    $"make" to build all plugins
    $"make FSAvionicsBridge" to build Plugin for MSFS
    $"make XPAvionicsBridgeWin" to build XP Plugin for Windows
    $"make XPAvionicsBridgeMac" to build XP Plugin for macOS
   
Alternatively, adapt the windows source files as required to build on Visual Studio or any other compiler / IDE.
