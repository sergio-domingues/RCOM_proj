#include "macros.h"

//recebe buffer CAMPO DE DADOS
int write_stuffing(int fd, const char * buffer,int length){
		
	int i,res = 0, acc = 0;
	//nao faz byte stuffing nas flags
	unsigned char send[2];
	for(i=1; i < length - 1; i++){	
	 
		if(buffer[i] == FLAG){
			send[0]=ESCAPE;
			send[1]=FLAG_STUFFING;
			res = write(fd,&send,2);
			acc += 2;			
		}else if(buffer[i] == ESCAPE){
			send[0]=ESCAPE;
			send[1]=ESCAPE_STUFFING;
			res = write(fd,&send,2);
			
			acc += 2;	
		}else {
			res = write(fd,&buffer[i],1);
			acc++;
		}
		
		if(res != 1)
			return -1;	
	}
	
	return acc;
}

int read_destuffing(int fd, char * data_to_be_filled){
	
	char ch[2];	
	int i = 0, res;
	
	while(1){ //nao ultrapassar o tamanho esperado
		
		res = read(fd,&ch,sizeof(ch));
		
		if(res <= 0){
			return -1;  //nao encontrou flag e terminou
		}
		else if(res == 1){
			if(ch[0] == FLAG){
				return i;
			}else
				return -1;
		}else { //res = 2
			if(ch[0] == FLAG ){
				return i;
			}
			else if(ch[1] == FLAG){
				data_to_be_filled[i] = ch[0];
				i++;
				return i;
			}			
		}		
				
		if(ch[0] == ESCAPE && ch[1] == FLAG_STUFFING ){  //#DESTUFF 1
			data_to_be_filled[i] = FLAG;
			i++;
		} else if (ch[0] == ESCAPE && ch[1] == ESCAPE_STUFFING){ //#DESTUFF 2
			data_to_be_filled[i] = ESCAPE;	
			i++;
		}
		else {
			data_to_be_filled[i] = ch[0];
			data_to_be_filled[i] = ch[1];
			i+=res;
		}
	}
	
	//nao encontrou flag
	return -1;
}
