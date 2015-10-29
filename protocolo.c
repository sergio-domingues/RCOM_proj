#include "macros.h"
#include "statemachine.h"
#include "stuffing.h"

#define FRAME_HEADER_SIZE 4  //BYTES
#define PACKET_HEADER_SIZE 

//nao colocar no ficheiro .h   (limited scope)
static int connection_transmitter(int fd);
static int connection_receiver(int fd);
static int disconnection_transmitter(int fd);
static int disconnection_receiver(int fd);

static int port_setting(int fd);

static int transmission_frame_SU(int fd, frame send,int length);	//loop send/receive till correct answer
static int send_frame(int fd, frame send,int length);
static int receive_frame(int fd, typeFrame* f);

static void atende(int signal);
static unsigned char calc_bcc(char* buffer, int length);
//================================
//>>>>GLOBALS

static struct termios oldtio; //boa pratica limited scope
static struct sigaction sa;
volatile static int alarm_flag = 0, counter = 0, num_sequencia = 0;

/* >>>>>   RESOLVER TODOS OS TODO'S <<<<<< */

//TODO CRIAR struct link layer para encapsular informacao
//no ficheiro .h declarar la a variavel e aqui declarar a descriçao da struct

//================================

int port_setting(int fd){	
	struct termios newtio;
	
    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }
	
	printf("Guarda configs porta.\n");
	
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = baudrate | CS8 | CLOCAL | CREAD;
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
	if (fd < 0) { perror(device_path); exit(-1); }
		
	printf("Inicia comunicao com dispostivo.\n");	
	
	port_setting(fd); //configura port
		
	 // Setup the sighub handler
    sa.sa_handler = &atende;	
	sigaction(SIGALRM, &sa, NULL);
	printf("setup SIGALARM handler.\n");	
	
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

// atende alarme
void atende(int signal){   
	alarm_flag = 1;
	counter++;           
	printf("alarme # %d\n", counter);	
}


int transmission_frame_SU(int fd, frame send, int length){
	
	typeFrame frame_received = DISC;
	counter = 0;
	
	//loop enqt (emissor not connected receiver) até max_retries	
	while(counter < MAX_RETRIES){
				
		if( send_frame(fd,send,length) <= 0 ){
			printf("Error sending frame.\n Trying to transmit again.\n");
			sleep(1);			
			counter++;
			continue;
		}

		printf("SET FRAME SENT.\n");
		
		//recebe UA frame com sucesso
		if( (receive_frame(fd, &frame_received) >= 0) && (frame_received == UA) ) { 
			printf("TRANSMITTER-RECEIVER connection established.\n");
			break;
		}
		else {
			counter++;
		}		
	}
	
	if(counter == MAX_RETRIES){ //nao conseguiu estabelecer conexao
		printf("connection not established.\n");
		counter = 0;
		return -1;
	}
	
	counter = 0;
	return 0; //success	
}

int transmission_frame_disc(int fd, frame send, int length){
	
	typeFrame frame_received = DISC;
	counter = 0;
	
	//loop enqt (emissor not connected receiver) até max_retries	
	while(counter < MAX_RETRIES){
				
		if( send_frame(fd,send,length) < 0 ){
			printf("Error sending frame.\n Trying to transmit again.\n");
			counter++;			
			continue;
		}

		printf("DISC SENT.\n");
		
		//RECEBE DISC
		if(receive_frame(fd, &frame_received) >= 0 && frame_received == DISC ){
			printf("DISC RECEIVED.\n");
		}
		else {
			counter++;
			continue;
		}

		//envio de UA 
		frame ua;
	
		ua.flag = FLAG;
		ua.a = A_EMI_REC;
		ua.c = C_UA;
		ua.bcc = A_EMI_REC^C_UA;
		ua.flag2 = FLAG;		
	
		if ( send_frame(fd,ua,sizeof(ua) ) < 0){
			printf("Erro na escrita da trama UA");		
			return -1;
		}	
		printf("disconnection: UA SENT.\n");
	}
	
	if(counter == MAX_RETRIES){ //nao conseguiu estabelecer conexao
		printf("disconnection not established.\n");
		counter = 0;
		return -1;
	}
	
	counter = 0;
	return 0; //success	
}




int connection_transmitter(int fd){		
	printf("CONNECTION TRANSMITTER.\n");	

	//envia trama SET
	frame set;
	
	set.flag=FLAG;
	set.a=A_EMI_REC;
	set.c=C_SET;
	set.bcc=A_EMI_REC^C_SET;
	set.flag2=FLAG;	
	
	//loop till send/receive succesfully
	if (transmission_frame_SU(fd,set,sizeof(set)) < 0){
		printf("Connection_transmitter: error/num_max_retransmissions transmitting frame.\n");
		return -1;
	}

	return fd;
}

int connection_receiver(int fd){
	typeFrame frame_received = SET;

	int i = 0;		
	while( i < MAX_RETRIES){
	
		/* WAIT FOR SET FRAME */
		if(receive_frame(fd,&frame_received) < 0){
			printf("connection_receiver: TIMEOUT on receive frame. Retransmitting\n");
			i++;	
			continue;		
		}

		if(frame_received == SET) 
			break;

		i++;		
	}

	if( i == MAX_RETRIES)  //ATINGIU NUMERO MAXIMO DE RETRANSMISSOES
		return -1;
	
	//envio de UA 
	frame ua;
	
	ua.flag = FLAG;
	ua.a = A_EMI_REC;
	ua.c = C_UA;
	ua.bcc = A_EMI_REC^C_UA;
	ua.flag2 = FLAG;		
	
	if ( send_frame(fd,ua,sizeof(ua) ) < 0){
		printf("Erro na escrita da trama UA");		
		return -1;
	}	
	else 
		printf("connection receiver:UA frame sent.\n");
	
	return fd;
}
//======================================

int send_frame(int fd, frame send,int length){
	return write(fd,&send,length);	
}

int receive_frame(int fd, typeFrame* f){	
	int pos_ack = 0;
	char ch;	
		
	alarm(ALARM_SPAN);  //activa alarme
	
	while(pos_ack == 0){
	    read(fd,&ch,1); //TODO: VERIFICAR RETORNO ??
	    printf("char recebido: 0x%X \n",(unsigned char)ch);
	    pos_ack = stateFunc(ch,f);
	    
	    if(alarm_flag){ //disparou alarme
			alarm_flag = 0;
			return -1;		
	    }
	}
	
	return pos_ack; //campo de controlo da trama, retornado pela statemachine
}

unsigned char calc_bcc( char* buffer, int length){	
	unsigned char bcc = 0;	
	int i;
	for(i=0; i < length; i++){	//4:IGNORA PRIMEIROS 4 BYTES (FLAG/A/C/BCC)
		bcc ^= buffer[i];
	}	
	return bcc;
}

//a ser chamada no emissor
int llwrite(int fd, char * buffer, int length){	
	//RECEIVES I = [PH|DATA]
	
	int ack, retries = 0;
	
	/* BEGINNING FRAME HEADER */
	
	char frame[FRAME_HEADER_SIZE + length + 2]; //2 = bcc2 + flag
	
	frame[0] = FLAG;
	frame[1] = A_EMI_REC;
	frame[2] = N(num_sequencia);   //a verificar conforme o valor retornado na resposta (rr,rej..)
	frame[3] = A_EMI_REC^frame[2];
	
	//num_sequencia = 1 - num_sequencia;  //actualiza valor para o esperado na resposta
	//============================		
	/* 	incorpora dados na trama 	*/	
	// I = [FH]	
	
	memcpy(&frame[4],buffer,length);  // I = [FH|PH|DATA]	

	//============================
	
	/* TAIL FRAME HEADER */
	
	frame[sizeof(frame)-2] = calc_bcc(buffer, length); //calcula bcc2 do campo de dados
	frame[sizeof(frame)-1] = FLAG;
	
	printf(">>>>>>>>>>>bcc : %d \n", frame[sizeof(frame)-2]);

	//I = [FH|PH|DATA|FT]
	//============================
	int chs_w;
	
	while( (retries < MAX_RETRIES) && (retries >= 0) ){	
		/*		 STUFFING 		*/		
		
		chs_w = write_stuffing(fd, frame, sizeof(frame));
		if (chs_w < 0){
			printf("error writting stuffed frame\n");
			retries++;
			continue;
		}

		printf(">>>>>chars writtend: %d.\n",chs_w);
		//============================		
		
		typeFrame r_frame = DISC; 				//valor de r_frame e modificado na statefunc
		
		
		/* 	ESPERA RESPOSTA DO RECEPTOR  */

		ack = receive_frame(fd,&r_frame);  		//valor do campo de controlo r, N(r)
		

		if(ack < 0){ 			/*   TIMEOUT   */
			printf("Timeout reached. Retry:%d.\n",retries);
		
			retries++;
			continue; 
		}	
		else{
			if(r_frame == RR){ //positive acknowledge				
				printf("Received RR\n");
				if( (ack >> 5) != num_sequencia){
					retries = -1; //termina ciclo  "sucesso"
					num_sequencia = 1 - num_sequencia;					
				} 
				else { 				//valor nao esperado
					counter++;
					continue; 		//tenta receber resposta com valor aceitavel
				}
			}
			else if (r_frame == REJ){	 					//rej acknowledge					
				printf("RECEIVED REJ.\n");

				if( (ack >> 5) != 1-num_sequencia){  		//ignore response
					printf("ignore response.\n");
					counter++;
					continue; 								//reenvia trama I
				}
				else {					
					//num_sequencia = 1-num_sequencia; 		//reset to previous value
					frame[2] = N(num_sequencia);   
					frame[3] = A_EMI_REC^frame[2];
					retries++;
					continue; 								//retransmit frame
				}
			}
			else {
				printf("Expected RR/REJ but received different frame.\n");
				retries++;
				continue; 
			}
		}		
	}
	
	if(retries == MAX_RETRIES)  //ATINGIU NUM MAXIMO DE RETRANSMISSOES
		return -1;
	
	return chs_w;
}

//a ser chamada no receptor
int llread(int fd, char * buffer){	
  
	typeFrame received_frame = DISC;
	int ack, ret;
	char s;
	
	frame reusable;
	reusable.flag = FLAG;
	reusable.a = A_EMI_REC;
	reusable.flag2 = FLAG;
	
	while(1){
	
		//VERIFICA O TIPO DE TRAMA RECEBIDA
		while(counter < 5){  //TODO REDEFINE HARDCODE VALUE
		
			/* 	 TENTA RECEBER TRAMA 	*/
			ack = receive_frame(fd, &received_frame);

			if(ack < 0){
				printf("llread:timeout reached.\n");
				counter++;
				sleep(1);
				continue;
			}

			/* RECEBE SET FRAME -> EMISSOR NAO RECEBEU UA NO LLOPEN */
			if(received_frame == SET ){
				
				reusable.c = C_UA;
				reusable.bcc = A_EMI_REC^C_UA;
				
				if ( send_frame(fd,reusable,sizeof(reusable)) < 0){
					printf("Erro na escrita da trama UA.\n");		
					return -1;
				}	
				else 
					printf("UA frame sent.\n");					
			}
			else if(received_frame == I){  // FRAME I
				printf("received I frame.\n");
				counter=0;
				s = ack >> 5; // ack -> campo de controlo da  trama
				printf("s:%d\n",s);
				break;  			
			}		
		}
		//nao recebeu frame
		if(counter == 5){ 
			counter = 0;
			printf("llread: didnt receive accpetable frame.\n");
			return -1; //TODO VERIFICAR SE ESTA E A OP + CORRECTA
		}
		counter = 0;
		
		if(s != num_sequencia){  //emissor nao recebeu PREVIOUS ACK 

			printf("ENVIA RR, ANTERIOR FOI PERDIDO.\n");

			s = 1-s; 		//actualzia apra o valor a ser enviado no RR 
			reusable.c = C_RR & N(s);
			reusable.bcc = A_EMI_REC^reusable.c;
			
			//reenvio do previous ack
			if(send_frame(fd,reusable,sizeof(reusable)) < 0){
				printf("Erro na escrita da trama REJ/RR.\n");		
				return -1; //TODO VER SE ESTE VALOR E INTERPRETADO CORRECTAMENT
			}
			continue;  //esperar por trama I			
		}
		
		//===============================
		/* TRATAMENTO DA TRAMA I  */
		
		ret = read_destuffing(fd,buffer);
		printf("Lidos:%d\n",ret);
		if(ret < 0 ){
			printf("llread:error on read_destuffing.\n");
			counter++;
			//sleep(1);
			continue;			
		}
		
		char bcc2;
		bcc2 = calc_bcc(buffer,ret-1); // -1: ignora bcc2
		printf(">>>>>>>>>>>>>>bcc2:%d\n",bcc2);
		
		
		//REJEITA FRAME <- ERRO NO BCC2
		if(bcc2 != buffer[ret-1] ){
			printf("llread: bcc is different.\n");

			printf("ENVIA TRAMA REJ.\n");
			
			reusable.c = C_REJ | N(s);
			reusable.bcc = A_EMI_REC^reusable.c;
		}
		else{
			num_sequencia = 1-num_sequencia;
			printf("ENVIA TRAMA RR.\n");
			
			reusable.c = C_RR | N(num_sequencia);
			reusable.bcc = A_EMI_REC^reusable.c;	
		}
		
		if(send_frame(fd,reusable,sizeof(reusable)) < 0){
			printf("Erro na escrita da trama REJ/RR.\n");		
			return -1;
		}
		else{	
			break;
		}
	}
	
	printf("llread.\n");
	return ret; //num caracteres lidos
}

//======================================
int llclose(int fd, int tipo){	 

	printf("LLCLOSE\n");
	int ret;

	if(tipo == TRANSMITTER){
		ret = disconnection_transmitter(fd);
	}else if(tipo == RECEIVER){
		ret = disconnection_receiver(fd);
	}else
		return -1;	


	if(ret < 0)
		return -1;
	
	sleep(5);

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {  //repor configs
      perror("tcsetattr");
      exit(-1);
    }

    if( close(fd) < 0 ) {
		printf("erro closing port.\n");
        return -1;
	}
		
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
	printf("try to send disc.\n");
	if ( transmission_frame_disc(fd,disc,sizeof(disc)) < 0 ) {
		printf("Error transmission/reception DISC's.\n");
		return -1;	
	}
	else
		printf("DISCONNECTED.\n");

	return 0; 
}

int disconnection_receiver(int fd){
	
	typeFrame frame_received = DISC, frame_received_2 = UA;
	
	frame rr;
	
	rr.flag = FLAG;
	rr.a = A_EMI_REC;
	rr.flag2 = FLAG;
	
	int ack;	
	
	//TRY TO receive DISC
	if( (ack = receive_frame(fd,&frame_received)) < 0 ) {
		printf("Receiving DISC timeout.\n");
		return -1;
	}
	else if(frame_received == I){  //emissor nao RECEBEU ULTIMO RR 
		printf("RECEIVER:Disc received.\n");
		
		int s = 1 -(ack >> 5);  //envia rr com campo s correcto
		
		rr.c = C_RR & N(s);
		rr.bcc = rr.a^rr.c;
		
		if( send_frame(fd,rr,sizeof(rr)) < 0 ){	//send DISC
			printf("Error sending DISC.\n");
			return -1;		
		}
	}		
	
	else if (frame_received != DISC )
		return -1;
	
	
	/*  	RECEBEU DISC -> ENVIA DISC E ESPERA UA		*/ 
	
	frame disc;
	
	disc.flag = FLAG;
	disc.a = A_REC_EMI;  //unica trama em que [a = A_REC_EMI]
	disc.c = C_DISC;
	disc.bcc = A_REC_EMI^C_DISC;
	disc.flag2 = FLAG;
	
	if( send_frame(fd,disc,sizeof(disc)) < 0 ){	//send DISC
		printf("Error sending DISC.\n");
		return -1;		
	}	
	printf("RECEIVER:Send DISC.\n");
		
	if( receive_frame(fd,&frame_received_2) < 0 ) {
		printf("Receiving UA timeout.\n");
		counter = 0;
		return -1;
	}
	else	
		fprintf(stderr,"RECEIVER:Received UA.\n");
	
	return 0;
}
