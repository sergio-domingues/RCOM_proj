#ifndef _STUFFING_H_
#define _STUFFING_H_

int write_stuffing(int fd, const char * buffer);
int read_destuffing(int fd, const char * data_to_be_filled);


#endif