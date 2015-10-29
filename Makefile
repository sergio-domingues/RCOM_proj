CC = gcc
FLAGS = -Wall -lm
FILES = link_layer.c stuffing.c app_layer_utils.c statemachine.c 
OUT_EXE = app

transmitter:
	$(CC) -o $(OUT_EXE) app_layer_transmitter.c $(FILES) $(FLAGS)

receiver:
	$(CC) -o $(OUT_EXE) app_layer_receiver.c $(FILES)  $(FLAGS)

clean:
	rm -f *.o
