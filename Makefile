CC = gcc
FLAGS = -Wall
FILES = protocolo.c stuffing.c statemachine.c
OUT_EXE = app

transmitter:
	$(CC) $(FLAGS) -o $(OUT_EXE) writenoncanonical.c $(FILES)

receiver:
	$(CC) $(FLAGS) -o $(OUT_EXE) noncanonical.c $(FILES)

clean:
	rm -f *.o