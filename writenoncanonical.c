/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "macros.h"
#include "protocolo.h"

/*volatile int STOP=FALSE;
int conta=1,flag = 1;
int pos_ack = 0;

void atende()                   // atende alarme
{
	printf("alarme # %d\n", conta);
	flag=1;
	conta++;
}
*/
//================================

int main(int argc, char** argv)
{
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS4", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

	int port;
	sscanf(argv[1],"/dev/ttyS%d",&port);

	int fd = llopen(port,TRANSMITTER);

	if(fd < 0){
		printf("error on llopen.\n");
		return -1;
	}
	
	//llwrite(fd,buffer,length);  
	
	if(llclose(fd,TRANSMITTER) < 0){
		printf("error on llclose.\n");
		return -1;
	}
	printf("acabou.\n");
	return 0;
    
}

