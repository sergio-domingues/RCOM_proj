#include "macros.h"
#include "protocolo.h"

#define FILENAME "pinguim.gif"
/*TODO ALTERAR ISTO PARA VALOR CORRECTO*/
#define FILE_LENGTH "11000"  
#define NUM_ARGS 2 

int main(int argc, char** argv){
  
  if(argc < NUM_ARGS + 1){
		printf("Usage: transmitter [PORT] [BAUDRATE].\n");
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
  
  int port_fd = argv[1];
  //=========================
  
  llopen(port_fd,RECEIVER);

	//LOOP
  //llread(...)  
  
   //para obter tamanho do ficheiro
  int file_size = 0;
  //recebe na primeira
  //TODO:ARG_SIZE(NUMERO DE BYTES QUE O TAMANHO DO FICHEIRO OCUPA) lido do pacote de controlo
  for(int i = 0 ; i < arg_size; i++){ 
	file_size += v[i]*pow(256,i);
  }
  //TODO FAZER ABORDAGEM SEMELHANTE PARA L2 E L1 DAS TRAMAS I
  
  //verifica que tipo de trama I se trata: I_DATA OU I_CONTROL_START OU I_CONTROL_END
  //SE FOR PACOTE DE CONTROLO CHAMAR CONTROL PACKET HEADER
  
  //TODO: VERIFICACOES
  //SE NUM_SEQ ACTUAL - NUM_SEQ_ANTERIOR > 1 -> SALTOU 1 FRAGMENTO
  //NO FINAL REPORTAR
  //ETC..
  
 
  
  llclose(port_fd, TRANSMITTER);
  
 return 0; 
}