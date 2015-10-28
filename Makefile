CC = gcc
FLAGS = -Wall -lm
FILES = protocolo.c stuffing.c statemachine.c
OUT_EXE = app

transmitter:
	$(CC) $(FLAGS) -o $(OUT_EXE) app_layer_transmitter.c $(FILES)

receiver:
	$(CC) $(FLAGS) -o $(OUT_EXE) app_layer_receiver.c $(FILES)

clean:
	rm -f *.o