#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7e
#define A 0x03
#define C_SET 0x07

#define TRANSMITTER 0 //TODO: verificar esses estes defines devem ser feitos aqui
#define RECEIVER 1
#define DISPOSITIVO "/dev/ttySx"

static struct termios oldtio; //boa prática limited scope

int llopen(int porta,int tipo){
	
	struct termios newtio;
	char device_path[strlen(DISPOSITIVO)+2]; // 2B : 1-portra 2-'\0'
	
	sprintf(device_path,"%s%d",DISPOSITIVO, porta);
		
	fd = open(device_path, O_RDWR | O_NOCTTY ); //If set & path identifies a terminal device dn't cause terminal device to become the controlling terminal for the process.
    if (fd <0) { perror(argv[1]); exit(-1); }
	
	printf("Inicia comunicao com dispostivo.\n")	
	
    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }
	printf("Guarda configs porta.\n");
	
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer 0 ms */
    newtio.c_cc[VMIN]     = 5;   /* blocking read till 5 chars read*/
	
	printf("Reconfigura porta.\n");	
	
	tcflush(fd, TCIOFLUSH); //flushes both data received but not read, and data written but not transmitted.

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
    printf("New termios structure set\n");

	//>>> TODO
	//COMPLETAR COM CHAMADAS ÀS FUNÇÕES QUE ENVIAM TRAMAS SET E UA CONFORME SEJA TRANSMITTER/RECEIVER
	//IMPLEMENTAR ESSAS FUNÇÕES NO BOTTOM FILE
	
    return 0;	
}

int llclose(int fd){	   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {  //repor configs
      perror("tcsetattr");
      exit(-1);
    }

    if(close(fd) < 0)
        return -1;
		
	printf("Termina ligacao com dispositivo.\n");	
	return 0;
}

int emissor(int fd){
	//envio de SET
	
	//recepção de UA
	//validação da trama com uso da maq de estados definida previamente
	
	return fd;
}

int receptor(int fd){
	//recepçao de SET
	//validaçao da trama com uso da maq de estados definida previamente
	
	//envio de UA 
	
	return 0;
}
