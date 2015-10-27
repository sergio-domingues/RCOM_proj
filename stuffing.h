#ifndef _STUFFING_H_
#define _STUFFING_H_

int write_stuffing(int fd, const unsigned char * buffer,int length);
int read_destuffing(int fd, unsigned char * data_to_be_filled, int length);

#endif