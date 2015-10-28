#ifndef _STUFFING_H_
#define _STUFFING_H_

int write_stuffing(int fd, char * buffer,int length);
int read_destuffing(int fd, char * data_to_be_filled);

#endif