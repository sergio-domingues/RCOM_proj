<<<<<<< HEAD
/*Non-Canonical Input Processing*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
/*    (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;*/
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

	int port;
	sscanf(argv[1],"/dev/ttyS%d",&port);

	int fd= llopen(port,RECEIVER);

	llclose(fd);

	return 0;
}

