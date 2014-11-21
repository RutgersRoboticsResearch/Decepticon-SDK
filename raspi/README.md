How to use this SDK
====================

This part of the SDK was made with the intention of being used on Raspberry Pis for communication to the arduino program located in a different folder.

<h2> Installing Prequisites </h2>

<h3> Dependencies: </h3>

	sudo apt-get update
	sudo apt-get upgrade
	sudo apt-get install cmake git gcc g++ libx11-dev libxt-dev libxext-dev libgraphicsmagick1-dev libopencv-dev
	git clone https://github.com/RutgersRoboticsResearch/Decepticon-SDK.git

<h3> Get the userland libraries: </h3>

	cd raspi/decepticon/
	git clone https://github.com/raspberrypi/userland.git
	cd userland
	./buildme

<h3> Compile robidouille's raspicam wrapper </h3>

	cd raspi/decepticon/robidouille/raspicam_cv
	make

<h2> Programming the intelligent agent </h2>

The agent you want to program is located in agent.cpp. Guides on how to use the SDK, as well as add your own sensor communication, is located inside slides/pdf provided.

<h3> Compile agent program </h3>

	cd raspi/
	make clean && make

<h3> Test the program </h3>

	./agent

<h3> Making sure it runs on autostart </h3>

	chmod a+x genautotermstart.sh
	./genautotermstart.sh
