#include "macros.h"
#include "app_layer_utils.h"

#define EXIT 5

void menu(int type){
	
	if(type != 0 && type != 1){
		return;	
	}

	//DEFAULT VALUES
	baudrate = 9600;
	MAX_RETRIES = 3;
	ALARM_SPAN = 3;

	if(type != RECEIVER)
		max_data_field = 1024;

	printf("==================> SETTINGS <======================\n");
	printf("BAUDRATE (DEFAULT:%d) 	- TO CHANGE PRESS 1\n", baudrate);
	printf("MAX_RETRIES (DEFAULT:%d) 	- TO CHANGE PRESS 2\n", MAX_RETRIES);
	printf("ALARM_SPAN (DEFAULT:%d) 		- TO CHANGE PRESS 3\n", ALARM_SPAN);
	printf("DATA FRAME SIZE:(DEFAULT:%d)     - TO CHANGE PRESS 4\n", max_data_field);
	printf("EXIT - PRESS 5  \n");
	printf("====================================================\n");

	int option,value;
	printf("option: ");
	scanf("%d",&option);


	while(option != EXIT) {  // LOOP TILL EXIT INPUT

		printf("value: ");

		switch(option){
			case 1: //baudrate
				scanf("%d",&value);

				if(value < BAUDRATE_MIN ||value > BAUDRATE_MAX){
					printf("Baudrate accepted values: [%d,%d].\n",BAUDRATE_MIN,BAUDRATE_MAX);
				}
				else
					baudrate = value;
			break;

			case 2: //max_retries
				scanf("%d",&value);

				if( value < 0 || value > MAX_RETRIES_MAX ){
					printf("Max Retries [0,%d]./n",MAX_RETRIES_MAX);
				}
				else
					MAX_RETRIES = value;
			break;

			case 3: //alarm span
				scanf("%d",&value);

				if(value < 0 || value > ALARM_SPAN_MAX){
					printf("Alarm Span [0,%d]./n",ALARM_SPAN_MAX);
				}
				else
					ALARM_SPAN = value;
			break;

			case 4: //data frame

				if(type == RECEIVER){
					printf("invalid option on TRANSMITTER\n\n");
					sleep(1);
				}
				else{
					scanf("%d",&value);

					if(value <= 0 || value > MAX_BUFFER_SIZE) {
						printf("Data's frames size ]0,%d]./n",MAX_BUFFER_SIZE);
					}
					else
						max_data_field = value;
				}
			break;

			case 5: //exit
				return;  
			break;

			default:
				printf("INVALID INPUT\n");
			break;
		}

		
		printf("==================> SETTINGS <======================\n");
		printf("BAUDRATE (DEFAULT: %d) 	- TO CHANGE PRESS 1\n", baudrate);
		printf("MAX_RETRIES (DEFAULT: %d) 	- TO CHANGE PRESS 2\n", MAX_RETRIES);
		printf("ALARM_SPAN (DEFAULT: %d) 	- TO CHANGE PRESS 3\n", ALARM_SPAN);
		printf("DATA FRAME SIZE (DEFAULT: %d)     - TO CHANGE PRESS 4\n", max_data_field);
		printf("EXIT - PRESS 5  \n");
		printf("====================================================\n\n");		
		printf("option: ");		
		scanf("%d",&option);
	}
	return;
}


int ctrl_packet_handler(char * buffer,control_packet_receiver* c_packet_array, int packet_index){	
	
	int i,indice=0,acc;

	for(i=0; i < 2; i++){  //2 - expect 2 args
	
		if(buffer[indice] == CTRL_ARG_FILE_NAME){ //parameter type
			indice++; 
			acc = buffer[indice]; //parameter Length
			indice++;
			memcpy(&c_packet_array[packet_index].file_name, &buffer[indice], acc); //copia nome para struct	
		}
		else if(buffer[indice] == CTRL_ARG_FILE_LENGTH){ //parameter type
			indice++; 
			acc = buffer[indice]; //parameter Length
			indice++;
			int file_size = 0, j;
			for( j = 0; j < acc; j++){ 
				file_size += (unsigned char)buffer[indice+j] * pow(256,j);
			}
			c_packet_array[packet_index].file_length = file_size;
		}
		else {
			printf("Control packet is corrupted.\n");
			return -1;
		}
		
		indice += acc;  //indice actual do buffer
	}
	
	return 0;
}

int data_packet_handler(char* buffer,int fd, int num_sequencia){
	
	if(buffer[0] - num_sequencia > 1 ){  //verifica valida do num sequencia
		printf("Expected frame sequence #%d but it was #%d.\n",num_sequencia+1,(unsigned char)buffer[0]);
		return -1;
	}
	
	int segment_size = (unsigned char)buffer[1]*256 + (unsigned char)buffer[2];  // l2*256 + l1
	
	/* WRITE FRAGMENT TO FILE DESTINATION */
	if (write(fd, buffer+3 , segment_size) < 0){  //ignora 3 primeiros bytes = n_seq + (l1 + l2) -> tamanho dos dados
		printf("Error writting file_segment.\n");
		return 1;
	}
	
	return 0;
}


void fillControlPacket(control_packet * packet){
    packet->control_field = I_CONTROL_START;  //START POR DEFEITO
 
    packet->type_filename = CTRL_ARG_FILE_NAME; //type : nome do ficheiro
 
    packet->type_file_length = CTRL_ARG_FILE_LENGTH; 	//tamanho do ficheiro
}

int changeToArray(control_packet packet, char* array){
     
     array[0]= packet.control_field;
     array[1] = packet.type_filename;
     array[2] = packet.length_filename;
     memcpy(&array[3], packet.value_filename, (int)packet.length_filename);
     
     array[3 + strlen(packet.value_filename)+1] = packet.type_file_length;
     array[4 + strlen(packet.value_filename)+1] = packet.length_file_length;
     memcpy(&array[5 + strlen(packet.value_filename)+1], packet.value_file_length, (int)packet.length_file_length);

     return (5 + (int)packet.length_filename +  (int)packet.length_file_length );    
}




