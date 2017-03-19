CC=cc
OUT=httpbench
FLAGS:= $(shell pkg-config --libs --cflags libcurl) -pthread

httpbench:
	$(CC) *.c $(FLAGS) -o $(OUT)

clean:
	rm -f *.o $(OUT)
