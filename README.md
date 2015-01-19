Interactive LED Mirror
======================

2048 interactive pixels with Raspberry PI and Raspicam

##How to build the software
This code runs on Raspbian. We use a model B second revision but it will probably run on any Pi. This code is based on a modified / stripped down version of raspivid. We found some very useful info on the website of Chris Cummings [http://www.cheerfulprogrammer.com/downloads/camcv.c](www.cheerfulprogrammer.com/downloads/camcv.c) and Pierre Raufast [http://raufast.org/download/camcv_vid0.c](http://raufast.org/download/camcv_vid0.c)

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

