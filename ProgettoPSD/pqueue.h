#ifndef PQUEUE_H
#define PQUEUE_H

#include "segnalazione.h"

/* Puntatore opaco per */
typedef struct pqueue_struct* PQueue;

/* Operatori */
PQueue crea_pqueue(void);
int is_empty_pq(PQueue q);
int insert_pq(PQueue q, Segnalazione s);
Segnalazione get_max_pq(PQueue q);
Segnalazione extract_max_pq(PQueue q);
void free_pqueue(PQueue q);

#endif