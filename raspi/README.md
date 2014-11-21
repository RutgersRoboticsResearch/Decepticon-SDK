Dependencies:

	sudo apt-get install libopencv-dev
	sudo apt-get install cmake git
	sudo apt-get install gcc g++ libx11-dev libxt-dev libxext-dev libgraphicsmagick1-dev

Get the userland libraries:

	cd decepticon/
	git clone https://github.com/raspberrypi/userland.git
	cd userland
	./buildme
	cd ..

Compile robidouille's raspicam wrapper

	cd robidouille/raspicam_cv
	make
