#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <macros.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define DISPOSITIVO "/dev/ttySx"

static struct termios oldtio; //boa pr�tica limited scope

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
    newtio.c_cc[VMIN]     = 1;   /* blocking read till 5 chars read*/
	
	printf("Reconfigura porta.\n");	
	
	tcflush(fd, TCIOFLUSH); //flushes both data received but not read, and data written but not transmitted.

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
    printf("New termios structure set\n");

	//>>> TODO
	//COMPLETAR COM CHAMADAS �S FUN��ES QUE ENVIAM TRAMAS SET E UA CONFORME SEJA TRANSMITTER/RECEIVER
	//IMPLEMENTAR ESSAS FUN��ES NO BOTTOM FILE
	
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
	struct frame set;
	set.flag=FLAG;
	set.a=A_EMI_REC;
	set.c=C_SET;
	set.BCC=A_EMI_REC^C_SET;
	set.flag2=FLAG;

	printf("Envio da trama");
	write(fd,set,sizeof(trama_SET));

	int pos_ack=0;
	char ch;
	

	//recep��o de UA
	
	while(pos_ack == 0){
	    res = read(fd,&ch,1);
		printf("%d ",res);
		printf("char recebido: %c \n",ch);
	    pos_ack=stateFunc(ch);
	}
	
	
	
	//valida��o da trama com uso da maq de estados definida previamente
	
	return fd;
}

int receptor(int fd){
	struct frame ua;
	ua.flag=FLAG;
	ua.a=A_REC_EMI;
	ua.c=C_UA;
	ua.BCC=A_REC_EMI^C_UA;
	ua.flag2=FLAG;


	//recep�ao de SET
	int pos_ack=0;
	char ch;
	void (*stateFunc)(char c) = start;
	
	while(pos_ack == 0){
	    res = read(fd,&ch,1);
		printf("%d ",res);
		printf("char recebido: %c \n",ch);
	    pos_ack=stateFunc(ch);
	}
	
	//valida�ao da trama com uso da maq de estados definida previamente
	write(fd,ua,sizeof(trama_SET));	


	//envio de UA 
	
	return 0;
}

int llclose(int fd){
	printf("Fechando ligacao");
	if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    	perror("tcsetattr");
      	exit(-1);
    }
	
	//enviar DIST receber DIST enviar UA
    close(fd);
    return 0;
}
