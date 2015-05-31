CC = gcc

all:
	@echo "make client or make cloudshell to choose which to compile"

client: client.c utils.c
	$(CC) client.c utils.c -o client

cloudshell: cloudshell.c parse_pidstat.c utils.c
	$(CC) cloudshell.c parse_pidstat.c utils.c -o cloudshell

clean:
	-rm client
	-rm cloudshell

