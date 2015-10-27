#include "macros.h"
#include "statemachine.h"

static frame receiveFrame;

/*Uso de parametro typeFrame*f necessario para 
statemachine tolerar casos de tramas I*/

extern int (*stateFunc)(char c,typeFrame* f)=&start;

int start(char c,typeFrame* f){
	printf("start\n");   		
	
	if(c == FLAG){
		stateFunc = flag_RCV;
		receiveFrame.flag = c;
	}
	return 0;
}

int flag_RCV(char c,typeFrame* f){
     printf("flag_RCV\n");	

     if(c == A_EMI_REC || c== A_REC_EMI){
	stateFunc = &A_RCV;
        receiveFrame.a = c;
     }
     else if ( c == FLAG)
	stateFunc = &flag_RCV;
     else 
	stateFunc = &start;
	
     return 0;
}

int A_RCV(char c,typeFrame* f){
	printf("A_RCV\n");
	
	int error = 0;
	
	//defining what type of frame was received
	switch(c){		
		case C_SET:
			*f = SET;
		break;

		case C_UA:
			*f = UA;
		break;

		case C_DISC:
			*f = DISC;
		break;

		case C_RR:
			*f = RR;			
		break;

		case C_REJ:
			*f = REJ;			
		break;
		
		case C_I_1:
			*f = I;
		break; 
				
		case C_I_0:
			*f = I;
		break;
		
		default:
			printf("StateMachine: Error with type of frame\n");
			error = 1;
		break;
	}
	
	//sucesso
	if(error == 0){
		stateFunc = &C_RCV;
		receiveFrame.c = c;	
		return 0;
	}
	
    if ( c == FLAG)
		stateFunc = &flag_RCV;
    else 
		stateFunc = &start;

	return 0;
}

int C_RCV(char c,typeFrame* f){
	printf("C_RCV\n");	
    if( c == (receiveFrame.a^receiveFrame.c)){
		stateFunc = &BCC;
		receiveFrame.bcc = c;
		
		if(*f == I){
			stateFunc = &start;
			//nao ha mais nada a receber (trama I)
			return c; //retorna valor campo controlo
		}		
	}
    else if ( c == FLAG)
		stateFunc = &flag_RCV;
    else 
		stateFunc = &start;

	return 0;
}

int BCC(char c,typeFrame* f){
printf("BCC\n");	 
    if (c == FLAG){
	    receiveFrame.flag2 = FLAG;
	    stateFunc = &stop;
	}
    else 
		stateFunc = &start;
	
    return 0;	
}

int stop(char c,typeFrame* f){
	printf("stop\n");
	stateFunc = &start;
	return receiveFrame.c; //valor a ser considerado nas calls	
}
