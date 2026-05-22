#ifndef HASH_H
#define HASH_H

#include "segnalazione.h"

/* Puntatore opaco */
typedef struct hash_table* HashTable;

/* Operatori base */
HashTable crea_hashtable(int size);
int insert_segnalazione_ht(HashTable ht, Segnalazione s);
Segnalazione search_segnalazione_ht(HashTable ht, int id);
int remove_segnalazione_ht(HashTable ht, int id);
void free_hashtable(HashTable ht);

/* Operatori avanzati per filtri e report */
void stampa_tutte_ht(HashTable ht);
void stampa_per_categoria_ht(HashTable ht, const char* categoria);
void stampa_per_stato_ht(HashTable ht, StatoSegnalazione stato);
void report_statistiche_ht(HashTable ht);

#endif
