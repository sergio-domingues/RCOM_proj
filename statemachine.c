#include "macros.h"
#include "statemachine.h"


static frame receiveFrame;

//=============================

int start(char c,typeFrame f){
	printf("start\n");   		
	
	if(c == FLAG){
		stateFunc = &flag_RCV;
		receiveFrame.flag = c;
	}
	return 0;
}

int flag_RCV(char c,typeFrame f){
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

int A_RCV(char c,typeFrame f){
	printf("A_RCV\n");

	switch(f){

	case SET:
		if(c == C_SET){
		stateFunc = &C_RCV;
		receiveFrame.c = c;	
		}
	break;

	case UA:
		if(c == C_UA){
		stateFunc = &C_RCV;
		receiveFrame.c = c;	
		}
	break;

	case DISC:
		if(c == C_DISC){
		stateFunc = &C_RCV;
		receiveFrame.c = c;	
		}
	break;

	case RR:
		if(c == C_RR){
		stateFunc = &C_RCV;
		receiveFrame.c = c;	
		}
	break;

	case REJ:
		if(c == C_REJ){
		stateFunc = &C_RCV;
		receiveFrame.c = c;	
		}
	break;

	default:
		printf("Error with type of frame");
	break;
	}

    if ( c == FLAG)
		stateFunc = &flag_RCV;
    else 
		stateFunc = &start;

	return 0;

}

int C_RCV(char c,typeFrame f){
	printf("C_RCV\n");	
    if( c == (receiveFrame.a^receiveFrame.c)){
		stateFunc = &BCC;
		receiveFrame.bcc = c;
	}
    else if ( c == FLAG)
		stateFunc = &flag_RCV;
    else 
		stateFunc = &start;

	return 0;
}

int BCC(char c,typeFrame f){
printf("BCC\n");	 
    if (c == FLAG){
	    receiveFrame.flag2 = FLAG;
	    stateFunc = &stop;
	}
    else 
	stateFunc = &start;
	
    return 0;	
}

int stop(char c,typeFrame f){
	printf("stop\n");
	stateFunc = &start;
	return 1;		
}
