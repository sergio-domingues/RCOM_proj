#include "macros.h"
#include "protocolo.h"

#define NUM_ARGS 2 

typedef struct{	
	int file_length;
	char file_name[30];	
} control_packet;

static control_packet c_packets[2];
static int file_descriptor, num_sequencia = 0;

int ctrl_packet_handler(char * buffer, int packet){	
	printf("control packet:%s.\n",buffer);
	
	int i,indice=0,acc;
	
	for(i=0; i < 2; i++){  //2 - expect 2 args
	
		if(buffer[indice] == CTRL_ARG_FILE_LENGTH){ //parameter type
			indice++; 
			acc = buffer[indice]; //parameter Length
			
			memcpy(&c_packets[packet], &buffer[indice], acc); //copia nome para struct	
		}
		else if(buffer[indice] == CTRL_ARG_FILE_NAME){ //parameter type
			indice++; 
			acc = buffer[indice]; //parameter Length
			
			int file_size = 0, j;
			for( j = 0; j < acc; j++){ 
				file_size += (unsigned char)buffer[indice+j] * pow(256,j);
			}
			c_packets[packet].file_length = file_size;
		}
		else {
			printf("Control packet is corrupted.\n");
			return -1;
		}
		
		indice += acc;  //indice actual do buffer
	}
	
	return 0;
}

int data_packet_handler(char* buffer){
	
	if(buffer[0] - num_sequencia > 1 ){  //verifica valida do num sequencia
		printf("Expected frame sequence #%d but it was #%d.\n",num_sequencia+1,(unsigned char)buffer[0]);
		return -1;
	}
	
	int segment_size = (unsigned char)buffer[1]*256 + (unsigned char)buffer[2];
	
	/* WRITE FRAGMENT TO FILE DESTINATION */
	if (write(file_descriptor, buffer+3 , segment_size) < 0){
		printf("Error writting file_segment.\n");
		return -1;
	}
	
	return 0;
}

int main(int argc, char** argv){
  
	if(argc < NUM_ARGS + 1){
		printf("Usage: app [PORT] [BAUDRATE].\n");
		return -1;
	}
	
	/* VERIFICACOES */
	
	if( atoi(argv[1]) < 0){
		printf("Insert correct port value.\n");
		return -1;
	}	
	int port = atoi(argv[1]);	
	
	if( atoi(argv[2]) < BAUDRATE_MIN || atoi(argv[2]) > BAUDRATE_MAX){
		printf("Baudrate accepted values: [%d,%d].\n",BAUDRATE_MIN,BAUDRATE_MAX);
		return -1;
	}	
	
	baudrate = atoi(argv[2]);	
	//=========================
    /* OPEN PORT AND CONNECTS */
	
	int port_fd;
    if ( (port_fd = llopen(port, RECEIVER)) < 0) { 
		printf("Error on llopen.\n");
		return -1;
	}
	//=========================
	
	int stop = 0;
	char buffer[MAX_BUFFER_SIZE];
	
	while(stop == 0){
		int res;
		res = llread(port_fd,buffer);
		
		if(res < 0){
			printf("Error in llread.\n");
			return -1;
		}
		
		int ret;
		
		switch (buffer[0]){
			
			case 0: //dados
				ret = data_packet_handler(&buffer[1]);					
			break;
			
			case 1: //start
				ret = ctrl_packet_handler(&buffer[1],0);
				
				/* OPEN FILE */
				file_descriptor = open(c_packets[0].file_name, O_CREAT|O_WRONLY, 0666);
				if(file_descriptor < 0){
					printf("Error opening file.\n");
					return -1;
				}
				printf("RECEIVER-File:%s opened.\n",c_packets[0].file_name);
			break;
				
			case 2:	//end	
				ret = ctrl_packet_handler(&buffer[1],1);

				if(close(file_descriptor) < 0){
					printf("Erro closing file.\n");
					return -1;
				}
				
				stop = 1;  //end while
			break;
			
			default:
				printf("Information is corrupted.\n");
				return -1;
			break;
		}
		
		if(ret < 0){
			printf("Error handling frame.\n");
			return -1;
		}
				
		num_sequencia++;
	}
	
	//TODO: VERIFICACOES
	
	//NO FINAL REPORTAR

  
	if(llclose(port_fd, TRANSMITTER) < 0){
		printf("Erro on llclose.\n");
		return -1;
	}
  
	return 0; 
}