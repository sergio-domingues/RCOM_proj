/*Non-Canonical Input Processing*/
#include "macros.h"
#include "protocolo.h"



int main(int argc, char** argv)
{ 
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

	int port;
	sscanf(argv[1],"/dev/ttyS%d",&port);

	int fd = llopen(port,RECEIVER);
	if(fd < 0){
		printf("error on llopen.\n");
		return -1;
	}
		
	if(llclose(fd,RECEIVER) < 0){
		printf("error on llclose.\n");
		return -1;
	}
	printf("acabou.\n");

	return 0;
}

