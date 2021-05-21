# RT Vision Demo 

## Version

Alpha0.1 Prototype

## Requirements

1. SEGGER J-Link Probe
2. SEGGER J-Link Software V6.32h or greater
3. NXP IoT & Security Solutions RT Vision board and RT1062 SOM board
4. USB Cable (Type C to Type A or Type C to Dual Type A)
5. MCUXpresso IDE v10.3.1 (previous or newer versions will likely work, but were not tested)
6. MCUXpresso SDK Version SDK_2.x_EVK-MIMXRT1060 Version 2.5.1 
   (visit https://mcuxpresso.nxp.com/en/select for information on how to install the MCUXpresso SDK)

## Setup

1. Connect the USB Type-C from a computer to the RT Vision Board.  If a Dual Type A cable is used, 
plug the longest Type A end into a power supply and plug the other Type A end to a computer.

2. Connect the J-Link Probe ribbon cable to the 10 pin header with VCC connected to pin 1.

3. Locate the RT Vision firmware and unzip if needed.

3. Install and start the MCUXpresso application. On startup you will be asked to create a new workspace.
Supply a location and name for the workspace in a writable location, NOT located in any of the RT Vision 
firmware subfolders. 

4. Once MCUXpresso starts, select File -> Import... -> General -> Existing Projects into Workspace, select Next >.
BROWSE to the location of the RT Vision firmware, UNSELECT Copy projects into workspace, if its already selected.  
SELECT Finish. SELECT Project -> Build All. 

5. After compiling is complete, SELECT the blue bug image on the icon menu to Start Debugging the Project 
with the active build configuration.  If no debug configuration exists, a default configuration will be created 
and the application will start and automatically stop at a breakpoint at the start of the function main(). SELECT 
the green circled arrow on the icon menu to launch the RT Vision application.







