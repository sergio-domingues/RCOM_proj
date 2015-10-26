#include "macros.h"
#include "protocolo.h"

#define FILENAME "pinguim.gif"
#define P_HEADER_SIZE 	4 			/* BYTES */
#define FILE_SECTION_MAX_SIZE 5000  /* BYTES */
#define NUM_ARGS 3

typedef struct ctrl_packet {
 unsigned char control_field = I_CONTROL_START;  //START POR DEFEITO
 
 unsigned char type_filename = CTRL_ARG_FILE_NAME; //nome do ficheiro
 unsigned char length_filename = sizeof(FILENAME);
 unsigned char value_filename[] = FILENAME;
 
 unsigned char type_file_length = CTRL_ARG_FILE_LENGTH; 	//tamanho do ficheiro
 unsigned char length_file_length;
 unsigned char* value_file_length;
 
} control_packet;

//============================

int main(int argc, char** argv){
  
	if(argc < NUM_ARGS + 1){
		printf("Usage: transmitter [PORT] [BAUDRATE] [FRAGMENT_SIZE BYTES].\n");
		return -1;
	}

	//TODO alterar isto para valor argv correcto
	
	/* VERIFICACOES */
	if(argv[1] < 0){
		printf("Insert correct port value.\n");
		return -1;
	}	
	int port_fd = argv[1];	
	
	if(argv[2] < BAUDRATE_MIN || argv[2] > BAUDRATE_MAX){
		printf("Baudrate accepted values: [%s,%s].\n",BAUDRATE_MIN,BAUDRATE_MAX);
		return -1;
	}	
	baudrate = argv[2];

	if(argv[3] < 0){
		printf("Introduce a correct value for data's frames max size./n");
		return -1;
	}	
	int max_data_field = argv[3]; 
  //===================
  
  /* OPEN PORT */
  
  //TODO VERIFICAR RETORNO
  llopen(port_fd,TRANSMITTER);  //se modificar 1º param, modificar tb llclose
  
  //============================
  
  /* OPEN FILE */
  int file_fd;
  if((file_fd = open(FILENAME,O_RDONLY)) < -1){
	  printf("Erro a abrir ficheiro %s.\n",FILENAME);
	  return -1;
  }

  //FILE'S INFO STRUCT
  struct stat fileStat;
  if(fstat(file,&fileStat) < 0){
	  printf("Erro a obter struct filestat.\n");
	  return -1;	  
  }    
  
  int file_size = fileStat.st_size; //TAMANHO DO FICHEIRO
  
  if(max_data_field > file_size){ 
	  printf("File section size input value is greater than file size, default value set.\n");
	  max_data_field = FILE_SECTION_MAX_SIZE;	  
  }
  //====================================== 
  /* 	 	DATA TRANSMISSION 			*/
  
  int seq_actual = 0;
	
  /* PACOTE DE CONTROLO (START) */
  control_packet c_packet_start;  			 //c-control d-data
  c_packet_start.length_file_length = sizeof(file_size);
  
  //converter inteiro em array de bytes
  unsigned char v[sizeof(file_size)]; 
  for(int i = 0; i < sizeof(file_size); i++){		
		v[i] = (x >> 8*i) & 0x00FF;
  }
  
  //em cada posicao do array esta um byte do tamanho shiftado
  c_packet_start.value_file_length = v;
  
  /* 		SENDING DATA CONTROL FRAME	     	*/
  //TODO: VERIFICAR RETORNO DO LLWRITE
  llwrite(port_fd, &c_packet_start, sizeof(c_packet_start));  //envio de packet start
  printf("packet start send\n");
  
  
  /* ENVIO DE SEGMENTOS DO FICHEIRO */  
  
  unsigned char data_packet[P_HEADER_SIZE + max_data_field], 
				buffer[max_data_field];
				
  int chs_read, stop = 0;
  
  //TODO MODIFICAR CONDICAO DO CICLO
  while(stop == 0){
 
	/* TRIES TO READ max_data_field BYTES FROM FILE */
	chs_read = read(file_fd,&buffer, max_data_field);  
	
	if(chs_read < 0){
		printf("Erro a ler ficheiro.\n");
		//TODO AGIR DE ACORDO
	}else if(chs_read < max_data_field){ //terminou ficheiro
		stop == 1;	//TERMINAR CICLO //TODO VERIFICAR ISTO	
	}	
		
	/* 	DATA PACKET HEADER 	*/
	data_packet[0] = 0;
	data_packet[1] = seq_actual;
	data_packet[2] = (0xFF00 &  chs_read) >> 8; //L2
	data_packet[3] = 0x00FF & chs_read;         //L1
	
	//incorpora fragmento do ficheiro |> I = [PH|___]
	memcpy(&data_packet[4], buffer, chs_read); //data_packet = [PH|DATA]
	
	//=================================
	
	int d_packet_length = P_HEADER_SIZE + chs_read; //PH_size + numero de chars lidos
	llwrite(port_fd,data_packet, d_packet_length);  
	
	//TODO
	//VERIFICAR RETORNO DO LLWRITE
	//SE RETORNO = POSITIVE ACK -> seq_actual++;
  }
  
   
 
  //ENVIA PACOTE DE CONTROLO (END)
  c_packet_start.control_field = 2;  //2 - END
  write(port_fd,&c_packet_start,sizeof(c_packet_start));  //envio de packet END
  //=====================================
  
  llclose(port_fd, TRANSMITTER);
  
 return 0; 
}