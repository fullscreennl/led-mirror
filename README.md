Interactive LED Mirror
======================

2048 interactive pixels with Raspberry PI and Raspicam

##How to build the software
This code runs on Raspbian. We use a model B second revision but it will probably run on any Pi. This code is based on a modified / stripped down version of raspivid. We found some very useful info on the website of Chris Cummings [http://www.cheerfulprogrammer.com/downloads/camcv.c](http://www.cheerfulprogrammer.com/downloads/camcv.c) and Pierre Raufast [http://raufast.org/download/camcv_vid0.c](http://raufast.org/download/camcv_vid0.c)

###1) Download / clone Raspberry Pi userland code
Create a folder src in your home dir and clone userland into this folder.

	mkdir src
	cd src
	git clone https://github.com/raspberrypi/userland.git
	
###2) Download / clone this repo
Now clone this code in your home dir.

	cd
	git clone https://github.com/fullscreennl/led-mirror.git
	
You now have this directory structure in your home dir:

	.
	├── src
	|   └── userland
	|
	└── led-mirror
	    ├── hardware
	    └── software

NOTE: If you prefer a different folder layout change the paths in the makefile. 	

###3) Build 
The software is in led-mirror/software

	cd led-mirror/software
	sudo make 
	
You now have a ledmirror binary

###4) Run

 	sudo ./ledmirror
 	
To start automatically on boot create an rc.local in /etc
 	
 	#!/bin/sh -e
	#
	# rc.local
	#
	# This script is executed at the end of each multiuser runlevel.
	# Make sure that the script will "exit 0" on success or any other
	# value on error.
	#
	# In order to enable or disable this script just change the execution
	# bits.
	#
	# By default this script does nothing.
	
	# Print the IP address
	_IP=$(hostname -I) || true
	if [ "$_IP" ]; then
	  printf "My IP address is %s\n" "$_IP"
	fi
	
	sudo /home/pi/led-mirror/software/ledmirror
	
	exit 0


##How to build the hardware

###Part List

###Power supply
* 5 Volt, 5 amp switching power supply, 1pcs
* Boxed enclosure, aluminium 15x25x10 cm, 1 pcs   
* Universal mains power cable
* Euro style mains receiving connector

###PCBs
* PCBs were designed in [Osmond PCB](http://www.osmondpcb.com) 
* designs can be found here [PCB designs](https://github.com/fullscreennl/led-mirror/tree/master/hardware)



###Main board
* LED DRIVERS: MAX6960, 16 pcs
* 4 MHz crystal, running at 3.3 Volts, 1 pcs
* High speed line buffer, NC7SZ126M5 (used to buffer the clock signal), 16 pcs
* 24-pin header, (this will be a 26 pin boxed header in the future pcb version), 16 pcs
* 26-pin polarized boxed header, interfacing with the Pi, 1 pcs
* power header, polarized 2 pin connector, 1 pcs
* Voltage regulator, LM3940 3.3Volt Low Dropout Regulator, 4 pcs
* 33µF, 25volts tantalum capacitor, balancing LM3940, 4 pcs
* 470 nF, ceramic capacitor, balancing LM3940,4 pcs
* +/- 47µF 25 volts, to decouple the MAX6960's, 16 pcs
* 0.1 µF ceramic caps, for each MAX powersupply pin, 80 pcs


###LED boards
* LEDS, WHITE 10MM LEDS (FORWARD voltage < 3.3 Volts, use 8 per PCB), 2048 pcs 
* Boxed Headers, 26 pin, 256 pieces
* Header 16 pins (2 on each PCB), 512 pcs
* Jumper in red color, 256 pcs


###Cables
* IDC female headers 26 pin, 256 (going to the LED panels)+ 16 (the main board end) + 2 (connecting the Pi to the main board)
* 26 wide ribbon cable, we used up and about 2.5 mters * 16 + 10 cm (Pi to Mainboard), roughly 40.5 meters
* 2 wire power cord, 5 meters

###Frame
* Metal tube, 6 cm diameter, 260 cm, 4 pcs
* Metal tube, 6 cm diameter, 200 cm, 10 pcs
* Tube couplings, Corner joint, 8 pcs
* Tube couplings, T joints, 4 pcs
* Grid panel, 244 X 122 cm, 3.8 cm pitched floor panel, 1 pcs 


###How to assemble

Warning this is a massive soldering job. We started by soldering the LED panel PCBs. Headers first and LEDS (other side of PCB) second. After that you can partially implement the Main Board, testing one chip setup at a time.

![Alt text](https://raw.github.com/fullscreennl/led-mirror/docs/images/driverboard_lrg.png "Mainboard desing (Osmond PCB)")  

![Alt text](https://raw.github.com/fullscreennl/led-mirror/docs/images/ledmodule_lrg.png "ledmodule design (Osmond PCB)")  


 


