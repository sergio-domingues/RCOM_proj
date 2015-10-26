#include "macros.h"
#include "protocolo.h"

#define FILENAME "pinguim.gif"
/*TODO ALTERAR ISTO PARA VALOR CORRECTO*/
#define FILE_LENGTH "11000"  


int main(int argc, char** argv){
  
  //ARGC E ARGVS... VERIFICACOES
  
  llopen(argv[1],RECEIVER);

	//LOOP
  //llread(...)  
  //verifica que tipo de trama I se trata: I_DATA OU I_CONTROL_START OU I_CONTROL_END
  //SE FOR PACOTE DE CONTROLO CHAMAR CONTROL PACKET HEADER
  
  //TODO: VERIFICACOES
  //SE NUM_SEQ ACTUAL - NUM_SEQ_ANTERIOR > 1 -> SALTOU 1 FRAGMENTO
  //NO FINAL REPORTAR
  //ETC..
  
  //para obter tamanho do ficheiro
  int file_size = 0;
  //TODO:ARG_SIZE(NUMERO DE BYTES QUE O TAMANHO DO FICHEIRO OCUPA) lido do pacote de controlo
  for(int i = 0 ; i < arg_size; i++){ 
	file_size += v[i]*pow(256,i);
  }
  
  //llclose();
  
 return 0; 
}