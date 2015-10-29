#include "macros.h"

//recebe buffer CAMPO DE DADOS
int write_stuffing(int fd, char * buffer,int length){
		
	int i,res = 0, acc = 0;
	//nao faz byte stuffing nas flags
	unsigned char send[2];
	
	write(fd,&buffer[0],1);  //flag inicial

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
			printf("stuff:%d\n",buffer[i]);
			res = write(fd,&buffer[i],1);
			acc++;
		}
		
		if(res < 1){
			printf("Error writing stuffing.\n");
			return -1;
		}	
	}

	write(fd,&buffer[length-1],1);  //flag final
	
	return acc;
}

int read_destuffing(int fd, char * data_to_be_filled){
	
	char ch, lastByte;
	int i = 0, res;
	
	while(1){ //nao ultrapassar o tamanho esperado
		
		res = read(fd,&ch,sizeof(ch));
		//printf("destuff:%X\n",(unsigned char) ch);
	
		if(res <= 0){
			printf("destuffing error.\n");
			return -1;  //nao encontrou flag e terminou
		}
		
		if(i == 0){
			lastByte = ch;
		}
		
		if(ch == FLAG){  //ENCONTROU FLAG -> TERMINA
			int j =0;			
			for(j=0;j<i;j++){
				printf("0x%02x ",(unsigned char)data_to_be_filled[j]);
			}printf("\n");


			return i;
		}
		
		if(lastByte == ESCAPE && ch == FLAG_STUFFING ){ 	//#DESTUFF 1
			data_to_be_filled[i] = FLAG;	
		} 
		else if (lastByte == ESCAPE && ch == ESCAPE_STUFFING){ 	//#DESTUFF 2
			data_to_be_filled[i] = ESCAPE;	
		}
		else if (ch != ESCAPE){
			data_to_be_filled[i] = ch;
		}else{
			i--;
		}		
		
		i++;
		lastByte = ch;
	}
	
	return -1;	//nao encontrou flag
}
