#include <
void start(char c,typeFrame frame);
void flag_RCV(char c,typeFrame frame);
void A_RCV(char c,typeFrame frame);
void C_RCV(char c,typeFrame frame);
void BCC(char c,typeFrame frame);
void stop(char c,typeFrame frame);


typedef enum {SET,DISC,UA,RR,REJ,I} typeFrame;

struct frame {
	 char flag;
	 char a;
	 char c;
	 char bcc;
	 char flag2;
};

static frame receiveFrame;

//TODO improve global
void (*stateFunc)(char c,typeFrame frame) = start;

void start(char c,typeFrame frame){
	printf("start\n");   		
	
	if(c == FLAG){
		stateFunc = flag_RCV;
		receiveFrame.flag = c;
	}
}

void flag_RCV(char c,typeFrame frame){
	printf("flag_RCV\n");	

     if(c == A_EMI_REC || c== A_REC_EMI){
	stateFunc = A_RCV;
        receiveFrame.a = c;
     }
     else if ( c == FLAG)
	stateFunc = flag_RCV;
     else 
	stateFunc = start;
}

void A_RCV(char c,typeFrame frame){
	printf("A_RCV\n");

	switch(frame){

	case SET:
		if(c == C_SET){
		stateFunc = C_RCV;
		receiveFrame.c = c;	
		}
	break;

	case UA:
		if(c == C_UA){
		stateFunc = C_RCV;
		receiveFrame.c = c;	
		}
	break;

	case DISC:
		if(c == C_DISC){
		stateFunc = C_RCV;
		receiveFrame.c = c;	
		}
	break;

	case RR:
		if(c == C_RR){
		stateFunc = C_RCV;
		receiveFrame.c = c;	
		}
	break;

	case REJ:
		if(c == C_REJ){
		stateFunc = C_RCV;
		receiveFrame.c = c;	
		}
	break;

	default:
		printf("Error with type of frame");
	break;
	}

    if ( c == FLAG)
		stateFunc = flag_RCV;
    else 
		stateFunc = start;
}

void C_RCV(char c,typeFrame frame){
	printf("C_RCV\n");	
    if( c == (receivedFrame.a^receivedFrame.c)){
		stateFunc = BCC;
		trama.bcc = c;
	}
    else if ( c == FLAG)
		stateFunc = flag_RCV;
    else 
		stateFunc = start;
}

void BCC(char c,typeFrame frame){
printf("BCC\n");	 
   	if (c == FLAG){
		receivedFrame.flag2 = FLAG;
		stateFunc = stop;
	}
    else 
		stateFunc = start;
}

void stop(char c,typeFrame frame){
	printf("stop\n");		
}
