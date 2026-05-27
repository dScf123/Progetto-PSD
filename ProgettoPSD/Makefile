CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Elenco dei file oggetto che comporranno il programma
OBJS = main.o segnalazione.o pqueue.o

# Obiettivo di default: compila entrambi
all: progetto test

# Compilazione del programma principale
progetto: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o progetto

# Compilazione del programma di test
test: main_test.o $(OBJS)
	$(CC) $(CFLAGS) -o test_progetto main_test.o $(OBJS)

main.o: main.c segnalazione.h
	$(CC) $(CFLAGS) -c main.c

main_test.o: main_test.c segnalazione.h hash.h pqueue.h
	$(CC) $(CFLAGS) -c main_test.c

segnalazione.o: segnalazione.c segnalazione.h
	$(CC) $(CFLAGS) -c segnalazione.c

pqueue.o: pqueue.c pqueue.h segnalazione.h
	$(CC) $(CFLAGS) -c pqueue.c

hash.o: hash.c hash.h segnalazione.h
	$(CC) $(CFLAGS) -c hash.c

clean:
	rm -f *.o progetto test_progetto progetto.exe test_progetto.exe