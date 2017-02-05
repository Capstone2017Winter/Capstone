CC=cc
OUT=httpbench
FLAGS:= $(shell pkg-config --libs --cflags libcurl)

httpbench:
	$(CC) *.c $(FLAGS) -o $(OUT)

clean:
	rm -f *.o $(OUT)
