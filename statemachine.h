#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

int start(char c,typeFrame f);
int flag_RCV(char c,typeFrame f);
int A_RCV(char c,typeFrame f);
int C_RCV(char c,typeFrame f);
int BCC(char c,typeFrame f);
int stop(char c,typeFrame f);

int (*stateFunc)(char c,typeFrame f) = &start;

#endif