How to use this SDK
====================

This part of the SDK was made with the intention of being used on Raspberry Pis for communication to the arduino program located in a different folder.

<h2> Installing Prequisites </h2>

<h3> Dependencies: </h3>

	sudo apt-get update
	sudo apt-get upgrade
	sudo apt-get install vim cmake git gcc g++ libx11-dev libxt-dev libxext-dev libgraphicsmagick1-dev libopencv-dev arduino
	git clone https://github.com/RutgersRoboticsResearch/Decepticon-SDK.git
	mv Decepticon-SDK/raspi .

<h3> Get the userland libraries: </h3>

	cd raspi/decepticon/
	git clone https://github.com/raspberrypi/userland.git
	cd userland
	./buildme

<h3> Compile robidouille's raspicam wrapper </h3>

	cd raspi/decepticon/robidouille/raspicam_cv
	mkdir objs
	make

<h2> Programming the intelligent agent </h2>

The agent you want to program is located in agent.cpp. To input your own sensors, just modify the output from the Arduino program. You can get the messages using get_arduino_message() from the Decepticon class. See reference.

<h3> Compile agent program </h3>

	cd raspi/
	make clean && make

<h3> Test the program </h3>

	./agent

<h3> Major Bug: Serial Garbage </h3>

During the development of this program, we realized that a large bug existed: during startup, the program would spit out garbage during serial reads. In order to stop this from happening, the Arduino program must be started up to read the serial data, then shut down. After that, the agent program runs smoothly.

We unfortunately do not have the time to find out why this occurs or how to fix it. We are sorry about this development.

<h2> Reference </h2>

This is the reference for the Decepticon class. The functions are shown in the header file <i>decepticon.hpp</i>.

	/* Easy to use functions */
	void forward();
	void backward();
	void turn_left();
	void turn_right();
	void stop();
	void open_claw();
	void close_claw();

	/* More precise control */
	void set_left(int speed);     /* min: -255, max: 255 */
	void set_right(int speed);    /* min: -255, max: 255 */
	void set_claw(int position);  /* min: 0, max: 180 */

	/* Get camera/ultrasonic data */
	cv::Mat take_picture();
	char *get_arduino_message();
	int get_sonar();
	bool opened();
