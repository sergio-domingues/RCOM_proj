<<<<<<< HEAD
/*Non-Canonical Input Processing*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7e
#define A 0x03
#define C_SET 0x07

volatile int STOP=FALSE;

void start(char c);
void flag_RCV(char c);
void A_RCV(char c);
void C_RCV(char c);
void BCC(char c);
void stop(char c);
void (*stateFunc)(char c) = start;

struct trama_SET {
	unsigned char flag;
	unsigned char a;
	unsigned char c;
	unsigned char bcc;
};

struct trama_SET trama;
int pos_ack = 0;

void clean_trama(){

     trama.flag = NULL;
     trama.a = NULL;
     trama.c = NULL;
     trama.bcc = NULL;
}
	
void start(char c){

	if(c == FLAG){
		stateFunc = flag_RCV;
		trama.flag = c;
	}
}

void flag_RCV(char c){

     if(c == A){
	stateFunc = A_RCV;
        trama.a = A;
     }
     else if ( c == FLAG)
	stateFunc = flag_RCV;
     else 
	stateFunc = start;
}

void A_RCV(char c){
	if(c == C_SET){
	stateFunc = C_RCV;
	trama.c = c;	
	}
     else if ( c == FLAG)
	stateFunc = flag_RCV;
     else 
	stateFunc = start;
}

void C_RCV(char c){

     if( c == (trama.a^trama.c))
	stateFunc = BCC;
     else if ( c == FLAG)
	stateFunc = flag_RCV;
     else 
	stateFunc = start;
}

void BCC(char c){
    if (c == FLAG)
	stateFunc = stop;
     else 
	stateFunc = start;
}

void stop(char c){
	pos_ack = 1;
	printf("recebeu no receptor\n");
}

//================================0

int main(int argc, char** argv)
{
int fd,c, res;
struct termios oldtio,newtio;
char buf[255];
if ( (argc < 2) ||
((strcmp("/dev/ttyS0", argv[1])!=0) &&
(strcmp("/dev/ttyS4", argv[1])!=0) )) {
printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
exit(1);
}
/*
Open serial port device for reading and writing and not as controlling tty
because we don't want to get killed if linenoise sends CTRL-C.
*/
fd = open(argv[1], O_RDWR | O_NOCTTY );
if (fd <0) {perror(argv[1]); exit(-1); }
tcgetattr(fd,&oldtio); /* save current port settings */
bzero(&newtio, sizeof(newtio));
newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
newtio.c_iflag = IGNPAR;
newtio.c_oflag = 0;
/* set input mode (non-canonical, no echo,...) */
newtio.c_lflag = 0;
newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
newtio.c_cc[VMIN] = 5; /* blocking read until 5 chars received */
tcflush(fd, TCIFLUSH);
tcsetattr(fd,TCSANOW,&newtio);
/* printf("New termios structure set\n"); */

//TP2

char ch;

while(pos_ack == 0){

     res = read(fd,ch,1);
     stateFunc(ch);
}


//TODO: introduzir sistema de alarme
//TODO: reenvio trama UA

write(fd,trama, sizeof(trama));


/*
res = 0;
while (STOP==FALSE) { 
res += read(fd,buf+res,1); 
if (buf[res-1] == '\0') {
STOP = TRUE;
}
}
printf("Read message: %s (%d bytes)\n", buf, res);
res = write(fd, buf, res);
printf("Message resent (%d bytes)\n", res);*/

sleep(5);
tcsetattr(fd,TCSANOW,&oldtio);
close(fd);
return 0;
}
=======
/*Non-Canonical Input Processing*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7e
#define A 0x03
#define C_SET 0x07

volatile int STOP=FALSE;

void start(char c);
void flag_RCV(char c);
void A_RCV(char c);
void C_RCV(char c);
void BCC(char c);
void stop(char c);
void (*stateFunc)(char c) = start;

struct trama_SET {
	char flag;
	char a;
	char c;
	char bcc;
	char flag2;
};

struct trama_SET trama;
int pos_ack = 0;

void clean_trama(){

     trama.flag = 0;
     trama.a = 0;
     trama.c = 0;
     trama.bcc = 0;
     trama.flag2= 0;
	
}
	
void start(char c){

printf("start\n");
	if(c == FLAG){
		stateFunc = flag_RCV;
		trama.flag = c;
	}


}

void flag_RCV(char c){

printf("flag_rcv\n");
     if(c == A){
	stateFunc = A_RCV;
        trama.a = A;
     }
     else if ( c == FLAG)
	stateFunc = flag_RCV;
     else 
	stateFunc = start;
}

void A_RCV(char c){

printf("a_rcv\n");
	if(c == C_SET){
	stateFunc = C_RCV;
	trama.c = c;	
	}
     else if ( c == FLAG)
	stateFunc = flag_RCV;
     else 
	stateFunc = start;
}

void C_RCV(char c){

printf("c_rcvvvv\n");
     if( c == (trama.a^trama.c)){
	trama.bcc= c;

	stateFunc = BCC;
}
     else if ( c == FLAG)
	stateFunc = flag_RCV;
     else 
	stateFunc = start;
}

void BCC(char c){

printf("???????\n");
    if (c == FLAG){
	stateFunc = stop;
	trama.flag2= FLAG;
	}
     else 
	stateFunc = start;
}

void stop(char c){

printf("test\n");
	pos_ack = 1;
	printf("recebeu no receptor\n");
}

//================================0

int main(int argc, char** argv)
{
int fd,c, res;
struct termios oldtio,newtio;
char buf[255];
if ( (argc < 2) ||
((strcmp("/dev/ttyS0", argv[1])!=0) &&
(strcmp("/dev/ttyS4", argv[1])!=0) )) {
printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
exit(1);
}
/*
Open serial port device for reading and writing and not as controlling tty
because we don't want to get killed if linenoise sends CTRL-C.
*/
fd = open(argv[1], O_RDWR | O_NOCTTY );
if (fd <0) {perror(argv[1]); exit(-1); }
tcgetattr(fd,&oldtio); /* save current port settings */
bzero(&newtio, sizeof(newtio));
newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
newtio.c_iflag = IGNPAR;
newtio.c_oflag = 0;
/* set input mode (non-canonical, no echo,...) */
newtio.c_lflag = 0;
newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
newtio.c_cc[VMIN] = 1; /* blocking read until 5 chars received */
tcflush(fd, TCIFLUSH);
tcsetattr(fd,TCSANOW,&newtio);
/* printf("New termios structure set\n"); */




//TP2

char ch;

	pos_ack = 0;

	while(pos_ack !=5){
	
	pos_ack++;
	     res = read(fd,&ch,1);
		printf("qual caractere: %x, n= %d \n", ch, res);
	     stateFunc(ch);
	}

/*
unsigned char chArray[5];

read(fd, chArray, sizeof(chArray));
printf("Array char %c \n", chArray); 
return 0;*/

//TODO: introduzir sistema de alarme
//TODO: reenvio trama UA

write(fd,&trama, sizeof(trama));


/*
res = 0;
while (STOP==FALSE) { 
res += read(fd,buf+res,1); 
if (buf[res-1] == '\0') {
STOP = TRUE;
}
}
printf("Read message: %s (%d bytes)\n", buf, res);
res = write(fd, buf, res);
printf("Message resent (%d bytes)\n", res);*/

sleep(5);
tcsetattr(fd,TCSANOW,&oldtio);
close(fd);
return 0;
}
>>>>>>> e0e15a29f8cba9cd683081965331900fc2ef6271
