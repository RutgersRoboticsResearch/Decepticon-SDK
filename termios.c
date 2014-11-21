/**
* (c) 2012, Manuel Di Cerbo, Nexus-Computing GmbH, www.nexus-computing.ch
*
* under GPLv2
* Thanks to
* Copyleft (c) 2006, Tod E. Kurt, tod@todbot.com
* http://todbot.com/blog/
*/

#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h> 
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>

#define BUFLEN 256

int serialport_init(const char* serialport);
void process(char* buffer, int len);

int
main (int argc, char *argv[])
{
	int fd = serialport_init("/dev/ttyACM0");
	if(fd < 0){//could not initialize port
		return -1;
	}
	
	char buffer[BUFLEN];
	int num = 0, len=0;
	
	
	for(;;){
		if(len == -1){
			perror("Could not execute write");
		}
		usleep(1E3*100);//100 [ms]
		num = read(fd, buffer, BUFLEN);//try to read buflen
		if(num == -1){
			usleep(1E3*10);//10 [ms]
			continue;
		}
		printf("read %d: %d\n", num, (unsigned char)buffer[0]);
	}
	
	return 0;
}

int serialport_init(const char* serialport)
{
	struct termios toptions;
	int fd;
	fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)  {
 		perror("init_serialport: Unable to open port ");
		return -1;
	}
	 
	if (tcgetattr(fd, &toptions) < 0) {
	perror("init_serialport: Couldn't get term attributes");
	return -1;
	}
	speed_t brate = B9600;

	cfsetispeed(&toptions, brate);
	cfsetospeed(&toptions, brate);

	//8N1
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	//no flow control
	toptions.c_cflag &= ~CRTSCTS;

	toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	toptions.c_oflag &= ~OPOST; // make raw

	// see: http://unixwiz.net/techtips/termios-vmin-vtime.html
	toptions.c_cc[VMIN]  = 0;
	toptions.c_cc[VTIME] = 20;

	if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
		perror("init_serialport: Couldn't set term attributes");
		return -1;
	}
	return fd;
}

