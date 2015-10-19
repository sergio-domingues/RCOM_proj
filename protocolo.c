#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "macros.h"

static struct termios oldtio; //boa prática limited scope

int llclose(int fd);
int llopen(int porta,int tipo);
int emissor(int fd);
int receptor(int fd);


int llopen(int porta,int tipo){
	
	struct termios newtio;
	char device_path[strlen(DISPOSITIVO)+2]; // 2B : 1-portra 2-'\0'
	
	sprintf(device_path,"%s%d",DISPOSITIVO, porta);
		
	int fd = open(device_path, O_RDWR | O_NOCTTY ); //If set & path identifies a terminal device dn't cause terminal device to become the controlling terminal for the process.
    if (fd <0) { perror(device_path); exit(-1); }
	
	printf("Inicia comunicao com dispostivo.\n");	
	
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
    newtio.c_cc[VMIN]     = 1;   /* blocking read till 5 chars read*/
	
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
	
	if(tipo==TRANSMITTER){
		return emissor(fd);
	}else if(tipo==RECEIVER){
		return receptor(fd);
	}else
		return -1;

    return 0;	
}


int emissor(int fd){
	frame set;
	set.flag=FLAG;
	set.a=A_EMI_REC;
	set.c=C_SET;
	set.bcc=A_EMI_REC^C_SET;
	set.flag2=FLAG;

	printf("Envio da trama");
	write(fd,&set,sizeof(frame));

	int pos_ack=0,res;
	char ch;
	

	//recepção de UA
	
	while(pos_ack == 0){
	    res = read(fd,&ch,1);
		printf("%d ",res);
		printf("char recebido: %c \n",ch);
	    pos_ack=stateFunc(ch);
	}
	
	return fd;
}

int receptor(int fd){
	frame ua;
	ua.flag=FLAG;
	ua.a=A_REC_EMI;
	ua.c=C_UA;
	ua.bcc=A_REC_EMI^C_UA;
	ua.flag2=FLAG;


	//recepçao de SET
	int pos_ack=0,res;
	char ch;
	
	while(pos_ack == 0){
	    res = read(fd,&ch,1);
		printf("%d ",res);
		printf("char recebido: %c \n",ch);
	    pos_ack=stateFunc(ch);
	}
	
	//validaçao da trama com uso da maq de estados definida previamente
	write(fd,&ua,sizeof(frame));	


	//envio de UA 
	
	return 0;
}
int llclose(int fd){	 
//enviar DIST receber DIST enviar UA  
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {  //repor configs
      perror("tcsetattr");
      exit(-1);
    }

    if(close(fd) < 0)
        return -1;
		
	printf("Termina ligacao com dispositivo.\n");	
	return 0;
}

