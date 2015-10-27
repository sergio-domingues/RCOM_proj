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

static unsigned char calc_bcc(const char* buffer, int length);
//================================
//>>>>GLOBALS

static struct termios oldtio; //boa pratica limited scope
static struct sigaction sa;
static int alarm_flag = 0, counter = 0;
static int num_sequencia = 0;

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
	if (fd <0) { perror(device_path); exit(-1); }
		
	printf("Inicia comunicao com dispostivo.\n");	
	
	port_setting(fd); //configura port
	
	
	 // Setup the sighub handler
    sa.sa_handler = &handle_signal;	
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
	printf("alarme # %d\n", counter);
	alarm_flag = 1;
	counter++;
	signal(SIGALRM, atende); //reinstala handler
}


int transmission_frame_SU(int fd, frame send,int length){
	
	typeFrame frame_received = DISC;
	
	//loop enqt (emissor not connected receiver) até max_retries	
	while(counter < MAX_RETRIES) {
				
		if( send_frame(fd,send,length) < 0 ){
			printf("Error sending SET frame.\n");
			exit(1);
		}
		
		if ( receive_frame(fd, &frame_received) != 0 ) { //recebe UA frame com sucesso
			printf("TRANSMITTER-RECEIVER (dis)connection established.\n");
			break;
		}		
	}
	
	if(counter == MAX_RETRIES){ //nao conseguiu estabelecer conexao
		printf("(dis)connection not established.\n");
		counter = 0;
		return -1;
	}
	counter = 0;
	return 0; //success	
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
	transmission_frame_SU(fd,set,sizeof(set));

	return fd;
}

int connection_receiver(int fd){

	//TODO: USAR TIMEOUT RETORNADO POR RECEIVE_FRAME PARA RETRANSMISSOES OU RETORNO

	//espera por uma trama SET correcta
	typeFrame frame_received = SET;
	
	receive_frame(fd,&frame_received);

	if(frame_received != SET) //TODO USAR VALOR RETORNADO
	  return -2;
		
	//envio de UA 
	frame ua;
	
	ua.flag = FLAG;
	ua.a = A_EMI_REC;
	ua.c = C_UA;
	ua.bcc = A_EMI_REC^C_UA;
	ua.flag2 = FLAG;		
	
	if ( send_frame(fd,ua,sizeof(ua)) < 0){
		printf("Erro na escrita da trama UA");		
		return -1;
	}	
	printf("UA frame sent.\n");
	
	return fd;
}
//======================================

int send_frame(int fd, frame send,int length){
	
	printf("Envio da trama\n length:%d\n",length);	
	return write(fd,&send,length);	
}


int receive_frame(int fd, typeFrame* f){
			
	counter = 0;	
	int pos_ack = 0, res;
	char ch;	
		
	alarm(ALARM_SPAN);  //activa alarme
	
	while(pos_ack == 0){
	    res = read(fd,&ch,1);
	    printf("char recebido: 0x%X \n",ch);
	    pos_ack = stateFunc(ch,f);
	    
	    if(alarm_flag){ //disparou alarme
			alarm_flag = 0;
			return -1;		
	    }
	}
	
	return pos_ack; //campo de controlo da trama, retornado pela statemachine
}

unsigned char calc_bcc(const char* buffer, int length){	
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
	
	unsigned char frame[FRAME_HEADER_SIZE + length + 2]; //2 = bcc2 + flag
	
	frame[0] = FLAG;
	frame[1] = A_EMI_REC;
	frame[2] = N(num_sequencia);   //a verificar conforme o valor retornado na resposta (rr,rej..)
	frame[3] = A_EMI_REC^frame[2];
	//============================		
	/* 	incorpora dados na trama 	*/	
	// I = [FH]	
	
	memcpy(&frame[4],buffer,length);  // I = [FH|PH|DATA]	
	//============================
	
	/* TAIL FRAME HEADER */
	
	frame[sizeof(frame)-2] = calc_bcc(buffer, length); //calcula bcc2 do campo de dados
	frame[sizeof(frame)-1] = FLAG;
	//I = [FH|PH|DATA|FT]
	//============================
	int chs_w;
	
	while( (retries < MAX_RETRIES) && retries > 0){	
		/*		 STUFFING 		*/		
		
		chs_w = write_stuffing(fd,frame, sizeof(frame));
		if (chs_w < 0){
			printf("error writting stuffed frame\n");
			return -1;
		}
		//============================		
		
		//espera resposta do receptor			
		int inner_responses = 0; //respostas aceitaveis
		
		//remover parte do while se estiver a dar coco
		while(inner_responses < MAX_RETRIES){ //no caso de receber resposta inesperada ("sem valor logico") 
			
			typeFrame r_frame = RR; //valor de r_frame e modificado na statefunc
			ack = receive_frame(fd,&r_frame);  //valor do campo de controlo r, N(r)
			
			if(ack < 0){ //espera por uma resposta do receptor
				printf("Timeout reached.\n");
				inner_responses++;
				continue; 
			}	
			else{
				if(r_frame == RR){ //positive acknowledge				
					
					if( (ack >> 5) != num_sequencia){
						retries = -1; //termina ciclo exterior "sucesso"
						break;
					} else { 		//valor nao esperado
						inner_responses++;
						continue; //tenta receber resposta com valor aceitavel
					}
				}
				else if (r_frame == REJ){	 //rej acknowledge					
					
					if( (ack >> 5) != num_sequencia){  //ignore response
						inner_responses++;
						continue; //tenta receber resposta com valor aceitavel
					}
					else {
						retries++;
						break; //retransmit frame
					}
				}
				else {
					printf("Expected RR/REJ but received different frame.\n");
					inner_responses++;
					continue; 
				}
			}
		}
		//nao recebeu resposta valida retorna programa
		if(inner_responses == MAX_RETRIES) 
			return -1;			
	}
	
	if(retries == MAX_RETRIES)  //ATINGIU NUM MAXIMO DE RETRANSMISSOES
		return -1;
	
	if(num_sequencia){//update num_se1
	  num_sequencia=0;
	}
	  else{
	    num_sequencia=1;
	  }
	return chs_w;
}

//a ser chamada no receptor
int llread(int fd, char * buffer){	
	//mais tarde completar com: store do numero de segmentos a ler (info recebida no 1º frame), etc
  
	//receber frame
	//realizar destuffing
  
	//verificar se é trama set 
	//se sim, enviar ua, esperar ate reeber trama I
		
	//qd receber frame I
	//ter em conta o L2 e L1
	
      //cast das cenas para unsigned char
  
	//remove FH E FT
	
	//verificacoes bbc2 
	
	//funcao receive_frame retorna campo de controlo C 
	
	//envio de resposta adequada
	//construcao de RR ou REJ
	//ter em conta o campo C da trama I para construir:
	//campo de controlo C = N(s) & (0010...) conforme seja RR ou REJ
	
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
	if ( transmission_frame_SU(fd,disc,sizeof(disc)) != 0 ) {
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
	if ( send_frame(fd,ua,sizeof(ua)) < 0 ){
		printf("Error sending UA");		
		return -1;
	}	
	printf("UA frame sent.\n");
	
	return 0; 
}

int disconnection_receiver(int fd){
	
	typeFrame frame_received = DISC, frame_received_2 = UA;
	//receive DISC
	receive_frame(fd,&frame_received);
	
	frame disc;
	
	disc.flag = FLAG;
	disc.a = A_REC_EMI;  //unica trama em que [a = A_REC_EMI]
	disc.c = C_DISC;
	disc.bcc = A_REC_EMI^C_DISC;
	disc.flag2 = FLAG;

	//TODO USAR RETRANSMISSIONS EM CASO DE TIMEOUT
	
	if( send_frame(fd,disc,sizeof(disc)) < 0 ){	//send DISC
		printf("Error sending DISC.\n");
		return -1;		
	}	
	printf("Send DISC.\n");
		
	if( receive_frame(fd,&frame_received_2) < 0 ) {
		printf("Receiving UA timeout.\n");
	}
	
	fprintf(stderror,"Receive UA.\n");
	counter = 0;
	
	return 0;
}
