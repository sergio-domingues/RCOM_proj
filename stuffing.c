#include "macros.h"

//recebe buffer CAMPO DE DADOS
int write_stuffing(int fd, const char * buffer,int length){
		
	int i,res = 0, acc = 0;
	//nao faz byte stuffing nas flags
	for(i=1; i < length - 1; i++){	
	
		if(buffer[i] == FLAG){
			res = write(fd,ESCAPE,1);
			res = write(fd,FLAG_STUFFING,1);
			acc += 2;			
		}else if(buffer[i] == ESCAPE){
			res = write(fd,ESCAPE,1);
			res = write(fd,ESCAPE_STUFFING,1);
			acc += 2;	
		}else {
			res = write(fd,buffer[i],1);
			acc++;
		}
		
		if(res != 1)
			return -1;	
	}
	
	return acc;
}

int read_destuffing(int fd, char * data_to_be_filled){
	
	char ch;
	int i = 0, length = strlen(data_to_be_filled);
	
	while(i < length ){ //nao ultrapassar o tamanho esperado
		
		read(fd,&ch,1);
		
		//realizar operacao de destuffing		
	}

	
	return 0;
}
