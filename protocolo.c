#include "macros.h"

//nao colocar no ficheiro .h   (limited scope)
static int connection_transmitter(int fd);
static int connection_receiver(int fd);
static int disconnection_transmitter(int fd);
static int disconnection_receiver(int fd);

static int port_setting(int fd);

static int transmission_frame_SU(int fd, frame send);	//loop send/receive till correct answer
static int send_frame(int fd, frame send);
static int receive_frame(int fd, typeFrame f);
//================================
//>>>>GLOBALS

static struct termios oldtio; //boa pratica limited scope
static int alarm_flag = 1, counter = 0;

//TODO CRIAR struct link layer para encapsular informacao
//no ficheiro .h declarar la a varaivel e aqui declarar a descriçao da struct

//================================

int port_setting(int fd){	
	struct termios newtio;
	
    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }
	
	printf("Guarda configs porta.\n");
	
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;	
    newtio.c_lflag = 0; 		/* set input mode (non-canonical, no echo,...) */

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer 0 ms */
    newtio.c_cc[VMIN]     = 1;   /* blocking read till 5 chars read*/
	
    printf("Reconfigura porta.\n");	
    
    tcflush(fd, TCIOFLUSH); //flushes both data received but not read, and data written but not transmitted.

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
	
    printf("New termios structure set\n");	
	return 0;	
}

int llopen(int porta,int tipo){	

	char device_path[strlen(DISPOSITIVO)+2]; // 2B : 1-porta 2-'\0'
	
	sprintf(device_path,"%s%d",DISPOSITIVO, porta);
		
	int fd = open(device_path, O_RDWR | O_NOCTTY ); //If set & path identifies a terminal device dn't cause terminal device to become the controlling terminal for the process.
	if (fd <0) { perror(device_path); exit(-1); }
		
	printf("Inicia comunicao com dispostivo.\n");	
	
	port_setting(fd); //configura port
	
	
	//CONNECTION================
	//ENVIO DE TRAMAS SET E UA
	if(tipo == TRANSMITTER){
		return connection_transmitter(fd);
	}else if(tipo == RECEIVER){
		return connection_receiver(fd);
	}else
		return -1;

    return fd;	
}


int connection_transmitter(int fd){	
	
	//envia trama SET
	frame set;
	
	set.flag=FLAG;
	set.a=A_EMI_REC;
	set.c=C_SET;
	set.bcc=A_EMI_REC^C_SET;
	set.flag2=FLAG;	
	
	//loop till send/receive succesfully
	transmission_frame_SU(fd,set);

	return fd;
}

// atende alarme
void atende(){              
	printf("alarme # %d\n", counter);
	alarm_flag = 1;
	counter++;
}

int transmission_frame_SU(int fd, frame send){
		  
	(void) signal(SIGALRM, atende); //instala handler para alarme
	
	typeFrame frame_received = UA;
	
	//loop enqt (emissor not connected receiver) até max_retries	
	while(counter < MAX_RETRIES) {
	
		if(alarm_flag){ 
		   alarm(ALARM_SPAN);  	//activa alarme de 3s                
		   alarm_flag = 0;
		}  			
			
		if( send_frame(fd,send) < 0 ){
			printf("Error sending SET frame.\n");
			exit(1);
		}
		
		if ( receive_frame(fd, frame_received) == 0 ) { //recebe UA frame com sucesso
			printf("TRANSMITTER-RECEIVER connection established.\n");
			break;
		}	
	}
	
	if(counter == MAX_RETRIES){ //nao conseguiu estabelecer conexao
		printf("connection not established.\n");
		return -1;
	}
			
	counter = 0;
	alarm_flag = 1;	
	
	return 0; //success	
}


int send_frame(int fd, frame send){
	
	printf("Envio da trama");	
	return write(fd,&send,sizeof(send));	
}


int receive_frame(int fd, typeFrame f){
	
	int pos_ack = 0, res;
	char ch;	
	
	while(pos_ack == 0){
	    res = read(fd,&ch,1);
	    printf("%d ",res);
	    printf("char recebido: %c \n",ch);
	    pos_ack = stateFunc(ch,f);
	    
	    if(alarm_flag) //disparou alarme
		return -1;		
	}
		
	return 0; //sucesso
}


int connection_receiver(int fd){	
	//espera por uma trama SET correcta
	typeFrame frame_received = SET;
	
	receive_frame(fd,frame_received);	
		
	//envio de UA 
	frame ua;
	
	ua.flag = FLAG;
	ua.a = A_EMI_REC;
	ua.c = C_UA;
	ua.bcc = A_EMI_REC^C_UA;
	ua.flag2 = FLAG;		
	
	if ( send_frame(fd,ua) < 0){
		printf("Erro na escrita da trama UA");		
		return -1;
	}	
	printf("UA frame sent.\n");
	
	return fd;
}
//======================================
//a ser chamada no emissor
int llwrite(int fd, char * buffer, int length){
	
	//TODO verificacao disto
	
	//recebe buffer com fragmento do ficheiro
	
	//constroi pacote de controlo para sinalizar 
	
	//constroi pacote de dados
	
				
	//envio de pacote de controlo
	
	//envio
	//
	
	//realizar operacao de stuffing aqui ?
	
	
	
	return 0; //res; //num chars lidos
}

//a ser chamada no receptor
int llread(int fd, char * buffer){	
	
  return 0; //return array_length; //num caracteres lidos
}

//======================================
int llclose(int fd, int tipo){	 
	
	if(tipo == TRANSMITTER){
		disconnection_transmitter(fd);
	}else if(tipo == RECEIVER){
		disconnection_receiver(fd);
	}else
		return -1;	


	sleep(5);

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {  //repor configs
      perror("tcsetattr");
      exit(-1);
    }

    if( close(fd) < 0 )
        return -1;
		
	printf("Termina ligacao com dispositivo.\n");	
	return 0;
}


int disconnection_transmitter(int fd){
	
	frame disc;
	
	disc.flag = FLAG;
	disc.a = A_REC_EMI;  //unica trama ao contrario
	disc.c = C_DISC;
	disc.bcc = A_REC_EMI^C_DISC;
	disc.flag2 = FLAG;	
	
	//loop send/receive DISC
	if ( transmission_frame_SU(fd,disc) != 0 ) {
		printf("Error transmission/reception DISC's.\n");
		return -1;	
	}
	printf("DISConnected.\n");
		
	frame ua;
	
	ua.flag = FLAG;
	ua.a = A_EMI_REC;
	ua.c = C_UA;
	ua.bcc = A_EMI_REC^C_UA;
	ua.flag2 = FLAG;		
	
	//chama send(ua)
	if ( send_frame(fd,ua) < 0 ){
		printf("Error sending UA");		
		return -1;
	}	
	printf("UA frame sent.\n");
	
	return 0; 
}

int disconnection_receiver(int fd){
	
	typeFrame frame_received = DISC, frame_received_2 = UA;
	//receive DISC
	receive_frame(fd,frame_received);
	
	frame disc;
	
	disc.flag = FLAG;
	disc.a = A_REC_EMI;  //unica trama em que [a = A_REC_EMI]
	disc.c = C_DISC;
	disc.bcc = A_REC_EMI^C_DISC;
	disc.flag2 = FLAG;

	(void) signal(SIGALRM, atende); //instala handler para alarme

	alarm(3);
	alarm_flag = 0;
	
	if( send_frame(fd,disc) < 0 ){	//send DISC
		printf("Error sending DISC.\n");
		return -1;		
	}	
	
	if( receive_frame(fd,frame_received_2) < 0 ) {
		printf("Receiving UA timeout.\n");
	}
		
	counter = 0;
	alarm_flag = 1;	
	
	return 0;
}