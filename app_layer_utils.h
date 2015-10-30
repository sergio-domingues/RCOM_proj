#ifndef _APP_LAYER_UTILS_H
#define _APP_LAYER_UTILS_H


typedef struct{	
	int file_length;
	char file_name[30];	
} control_packet_receiver;

typedef struct {
 unsigned char control_field;  //START POR DEFEITO
 
 unsigned char type_filename; //nome do ficheiro
 unsigned char length_filename;
 char * value_filename;
 
 unsigned char type_file_length; 	//tamanho do ficheiro
 unsigned char length_file_length;
 unsigned char* value_file_length;
 
} control_packet;

void menu(int type);

int ctrl_packet_handler(char * buffer, control_packet_receiver* c_packet_array, int packet_index);
int data_packet_handler(char* buffer, int fd, int num_sequencia);

void fillControlPacket(control_packet * packet);
int changeToArray(control_packet packet, char* array);

#endif
