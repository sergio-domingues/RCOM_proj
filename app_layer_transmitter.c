#include "macros.h"
#include "protocolo.h"

#define FILENAME "pinguim.gif"
/*TODO ALTERAR ISTO PARA VALOR CORRECTO*/
#define FILE_LENGTH "11000" 
#define PACKET_HEADER_LENGTH 	4 	/*BYTES*/

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

int main(int argc, char** argv){
  
  //ARGC E ARGVS... VERIFICACOES
  
  //llopen(...)

  //>>>> DATA TRANSMITTING SECTION <<<<
  //TODO CALCULAR ISTO
  int tam_max_fragmento = 1000; //BYTES
  int num_fragmentos = 20;
  int seq_actual = 0;
  
  
  //START PACKET
  control_packet c_packet_start;  			 //c-control d-data
  write(fd, &c_packet_start, sizeof(c_packet_start));  //envio de packet start
  
  
  unsigned char data_packet[tam_max_fragmento + PACKET_HEADER_LENGTH];
  unsigned char data_header[PACKET_HEADER_LENGTH];
  
  data_header[0] = 0;
  data_header[1] = seq_actual;
  data_header[2] = (0xFF00 & tam_max_fragmento) >> 8; //L2
  data_header[3] = 0x00FF & tam_max_fragmento;        //L1
  
  while(1){
    
     
    //llwrite(fd,
    //SE RECEBE POSITIVE ACK -> seq_actual++
  }
  
  /*END PACKET*/
  c_packet_start.control_field = 2;  //2 - END
  write(fd,&c_packet_start,sizeof(c_packet_start));  //envio de packet END
  //=====================================
  
  //llclose();
  
 return 0; 
}