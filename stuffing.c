#include "macros.h"

//recebe buffer CAMPO DE DADOS
int write_stuffing(int fd, char * buffer,int length){
		
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
		
		if(res < 1){
			printf("Error writing stuffing.\n");
			return -1;
		}	
	}
	
	return acc;
}

int read_destuffing(int fd, char * data_to_be_filled){
	
	char ch, lastByte;
	int i = 0, res;
	
	while(1){ //nao ultrapassar o tamanho esperado
		
		res = read(fd,&ch,sizeof(ch));		
		//printf("destuff: %d\n",ch);
		if(res <= 0){
			printf("destuffing error.\n");
			return -1;  //nao encontrou flag e terminou
		}
		
		if(i == 0){
			lastByte = ch;
		}
		
		if(ch == FLAG){  //ENCONTROU FLAG -> TERMINA
			return i;
		}
		
		if(lastByte == ESCAPE && ch == FLAG_STUFFING ){ 	//#DESTUFF 1
			data_to_be_filled[i] = FLAG;	
		} 
		else if (lastByte == ESCAPE && ch == ESCAPE_STUFFING){ 	//#DESTUFF 2
			data_to_be_filled[i] = ESCAPE;	
		}
		else{
			data_to_be_filled[i] = ch;
		}		
		
		i++;
		lastByte = ch;
	}
	
	return -1;	//nao encontrou flag
}
