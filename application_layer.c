#include "macros.h"
#include "protocolo.h"

#define FILENAME "pinguim.gif"
/*TODO ALTERAR ISTO PARA VALOR CORRECTO*/
#define FILE_LENGTH "11000"  

typedef struct ctrl_packet {
  
 char control_field = 1;  //START POR DEFEITO
 
 char type_filename = 1; //nome do ficheiro
 char length_filename = sizeof(FILENAME);
 char value_filename[] = FILENAME;
 
 char type_file_length = 0 //tamanho do ficheiro
 char length_file_length = sizeof(FILE_LENGTH);
 char value_file_length = FILE_LENGTH;
  
} control_packet;


//TODO 
int main(int argc, char** argv){
  
  //llopen(...)

  
  control_packet c_packet_start;  //c-control d-data

  //envio de packet start
  write(fd,&c_packet_start,sizeof(c_packet_start));
    
  
  
  
  //llclose();
  
 return 0; 
}