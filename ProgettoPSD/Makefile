CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Elenco dei file oggetto che comporranno il programma
OBJS = main.o segnalazione.o pqueue.o

all: progetto

progetto: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o progetto

main.o: main.c segnalazione.h
	$(CC) $(CFLAGS) -c main.c

segnalazione.o: segnalazione.c segnalazione.h
	$(CC) $(CFLAGS) -c segnalazione.c

pqueue.o: pqueue.c pqueue.h segnalazione.h
	$(CC) $(CFLAGS) -c pqueue.c

hash.o: hash.c hash.h segnalazione.h
	$(CC) $(CFLAGS) -c hash.c

clean:
	rm -f *.o progetto progetto.exe