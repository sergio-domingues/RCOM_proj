#include "macros.h"
#include "link_layer.h"
#include "app_layer_utils.h"

#define NUM_ARGS 1

static control_packet_receiver c_packets[2];  //control packet start and end
static int file_descriptor, num_sequencia = 0;

int main(int argc, char** argv){
  
	if(argc < NUM_ARGS + 1){
		printf("Usage: app [PORT].\n");
		return -1;
	}
	
	/* VERIFICACOES */
	
	if( atoi(argv[1]) < 0){
		printf("Insert correct port value.\n");
		return -1;
	}	
	int port = atoi(argv[1]);	
	
	menu(RECEIVER);  //MENU

	//=========================
    /* OPEN PORT AND CONNECTS */
	
	int port_fd;
    if ( (port_fd = llopen(port, RECEIVER)) < 0) { 
		printf("Error on llopen.\n");
		return -1;
	}
	//=========================
	//STATS RELATED
	int num_i_frames = 0;


	int stop = 0, cnt = 0;
	char buffer[MAX_BUFFER_SIZE];
	
	while( (stop == 0) && (cnt < MAX_RETRIES)){ 
		int res;
		
		res = llread(port_fd,buffer);
		
		if(res < 0){
			printf("Error in llread.\n");
			
			cnt++;
			sleep(1);
			continue;			
		}
		
		int ret;
		
		switch (buffer[0]){
			
			case 0: //dados
				ret = data_packet_handler(&buffer[1],file_descriptor, num_sequencia);
				if(ret < 0){
					printf("error writting to file.\n");
				}
				printf("data packet handler.\n");
				num_i_frames++;		//stats			
			break;
			
			case 1: //start
				ret = ctrl_packet_handler(&buffer[1], c_packets, 0); //0 - start
				printf("ctrl packet handler start.\n");	
				
				/* OPEN FILE */

				file_descriptor = open(c_packets[0].file_name, O_CREAT|O_WRONLY, 0666);
				if(file_descriptor < 0){
					printf("Error opening file.\n");
					return -1;
				}
				printf("RECEIVER-File:%s opened.\n",c_packets[0].file_name);
			break;
				
			case 2:	//end	
				ret = ctrl_packet_handler(&buffer[1], c_packets, 1); // 1 - end
				printf("crtl packet handler end.\n");	

				if(close(file_descriptor) < 0){
					printf("Erro closing file.\n");
					return -1;
				}
				
				stop = 1;  //end while
			break;
			
			default:
				printf("Information is corrupted.\n");
				return -1;
			break;
		}
		
		if(ret < 0){
			printf("Error handling frame.\n");
			return -1;
		}
				
		num_sequencia++;
	}

	if(cnt == MAX_RETRIES)
		return -1;	
	

	printf("\n\nSTATS:\n");
	print_stats();
	printf("I frames received: %d\n",num_i_frames);
	
	//NO FINAL REPORTAR

  
	if(llclose(port_fd, RECEIVER) < 0){
		printf("Erro on llclose.\n");
		return -1;
	}
  
	return 0; 
}
