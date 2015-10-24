#include "macros.h"
#include "protocolo.h"

#define FILENAME "pinguim.gif"
/*TODO ALTERAR ISTO PARA VALOR CORRECTO*/
#define FILE_LENGTH "11000" 
#define P_HEADER_SIZE 	4 	/*BYTES*/

typedef struct ctrl_packet {
 unsigned char control_field = 1;  //START POR DEFEITO
 
 unsigned char type_filename = 1; //nome do ficheiro
 unsigned char length_filename = sizeof(FILENAME);
 unsigned char value_filename[] = FILENAME;
 
 unsigned char type_file_length = 0 //tamanho do ficheiro
 unsigned char length_file_length = sizeof(FILE_LENGTH);
 unsigned char value_file_length = FILE_LENGTH;  
 
} control_packet;


//============================

unsigned char calc_bcc(const char* buffer, int length){
	
	unsigned char bcc = 0;
	
	int i;
	for(i=0; i < length; i++){		
		bcc ^= buffer[1];
	}
	
	return bcc;
}


int main(int argc, char** argv){
  
  //ARGC E ARGVS... VERIFICACOES
  
  //llopen(...)

  //>>>> DATA TRANSMISSION SECTION <<<<
  //TODO CALCULAR ISTO
  int tam_fragmento= 1000; //BYTES
  int num_fragmentos = 20;
  int seq_actual = 0;
  int data_packet_length = tam_fragmento + P_HEADER_SIZE;
  
  
  //PACOTE DE CONTROLO (START)
  control_packet c_packet_start;  			 //c-control d-data
  write(fd, &c_packet_start, sizeof(c_packet_start));  //envio de packet start
  printf("packet start\n");
  
  
  unsigned char data_packet[tam_fragmento + P_HEADER_SIZE];
  
  //DATA PACKET HEADER
  data_packet[0] = 0;
  data_packet[1] = seq_actual;
  data_packet[2] = (0xFF00 &  tam_max_fragmento) >> 8; //L2
  data_packet[3] = 0x00FF & tam_max_fragmento;         //L1
  
  while(1){ //HANDLER TRAMAS I 
    printf("ciclo while\n");
     
    llwrite(fd,data_packet,sizeof(data_packet));
    break; 
    //TODO testar a receber apenas 1 packet
    
    //SE RECEBE POSITIVE ACK -> seq_actual++
  }
  
  //PACOTE DE CONTROLO (END)
  c_packet_start.control_field = 2;  //2 - END
  write(fd,&c_packet_start,sizeof(c_packet_start));  //envio de packet END
  //=====================================
  
  //llclose();
  
 return 0; 
}