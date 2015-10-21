/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "macros.h"

/*volatile int STOP=FALSE;
int conta=1,flag = 1;
int pos_ack = 0;

void atende()                   // atende alarme
{
	printf("alarme # %d\n", conta);
	flag=1;
	conta++;
}
*/
//================================

int main(int argc, char** argv)
{
/*    (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;*/
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS4", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

	int port;
	sscanf(argv[1],"/dev/ttyS%d",&port);

	int fd = llopen(port,TRANSMITTER);

	//llwrite(fd,buffer,length);  

	llclose(fd);

	return 0;

    //tp2
/*	unsigned char SET[5];
	SET[0]=FLAG;
	SET[1]= A; //A
	SET[2]= C_SET; //C
	SET[3]= SET[1]^SET[2]; //BCC
	SET[4]= FLAG;

	int v ;
	char ch;

	while(conta < 4){
		
		if(flag){
		   alarm(3);                 // activa alarme de 3s
		   flag=0;
		}  	
		
		v = write(fd,SET,5); //envia trama	
		printf("escreve %d bytes:\n",v);			
		
		while(pos_ack == 0){
		     	res = read(fd,&ch,1);
			printf("char recebido: %d \n",ch);
			stateFunc(ch);
			if(flag==1)
			   break;
		}
		if(pos_ack == 1)  //recebe trama correctamente
			break;		
	}




/*
    char ch;
    for (i = 0; i < 255; i++) {
	
	ch= (char) getchar();      
	if(ch == '\n'){
		buf[i]='\0';
		break;
	}
	else{
	buf[i]=ch;
	
    	}	
	}
     
    res = write(fd,buf,255);   
    printf("%d bytes written\n", 

	res = 0;
while (STOP==FALSE) { 
res += read(fd,buf+res,1);
if (buf[res-1] == '\0') {
STOP = TRUE;
}
}
printf("%s\n",buf);	
printf("%d bytes written\n", res);
*/




/*	sleep(5);

   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }




    close(fd);

*/
    
}

