CC=cc
OUT=httpbench
FLAGS:= -I$(CURDIR)/curl-7.52.1../include -L$(CURDIR)/curl-7.52.1../lib -lcurl -pthread -std=c99

httpbench:
	$(CC) *.c $(FLAGS) -o $(OUT)

clean:
	rm -f *.o $(OUT)
