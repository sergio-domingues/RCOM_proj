#ifndef _PROTOCOLO_H
#define _PROTOCOLO_H

int llopen(int porta,int tipo);
int llwrite(int fd, char * buffer, int length);
int llread(int fd, char * buffer);
int llclose(int fd, int tipo);


#endif