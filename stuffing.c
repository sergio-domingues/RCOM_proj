#include "macros.h"

//recebe buffer CAMPO DE DADOS
int write_stuffing(int fd, const char * buffer){
	
	int length = strlen(buffer);
	
	int i;
	for(i=0; i < length; i++){		
		
		if(buffer[i] == FLAG){
			write(fd,ESCAPE,1);
			write(fd,FLAG_STUFFING,1);
						
		}else if(buffer[i] == ESCAPE){
			write(fd,ESCAPE,1);
			write(fd,ESCAPE_STUFFING,1);
			
		}else {
			write(fd,buffer[i],1);
		}		
	}
	
	
	
	
	return 0;
}

int read_destuffing(int fd){
	
	
	
	
	
	return 0;
}
