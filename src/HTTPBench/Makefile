CC=cc
OUT=httpbench
FLAGS:= -I/curl-7.52.1-built/include -L/curl-7.52.1-built/lib -lcurl -pthread

httpbench:
	$(CC) *.c $(FLAGS) -o $(OUT)

clean:
	rm -f *.o $(OUT)
