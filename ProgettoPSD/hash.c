#define _CRT_SECURE_NO_WARNINGS /* Per poter usare strcpy */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

/* gestisco le collisioni tramite Chaining */
struct hash_node {
    Segnalazione s;
    struct hash_node* next;
};

/* * Contiene la dimensione e un array dinamico di puntatori a hash_node */
struct hash_table {
    int size;
    struct hash_node** table;
};

HashTable crea_hashtable(int size) {
    /* Alloco la memoria per la struttura base */
    HashTable ht = (HashTable)malloc(sizeof(struct hash_table));
    if (!ht) return NULL;

    ht->size = size;

    /* Utilizzo il cast (size_t) per prevenire in caso di overflow se 'size' dovesse essere molto grande */
    ht->table = (struct hash_node**)malloc(sizeof(struct hash_node*) * (size_t)size);

    if (!ht->table) {
        free(ht); /* libero la memoria allocata precedentemente per evitare leak */
        return NULL;
    }

    /* Inizializzo tutti i puntatatori a NULL (liste attualmente vuote) */
    for (int i = 0; i < size; i++) {
        ht->table[i] = NULL;
    }

    return ht;
}

/* Utilizza l'operatore modulo per mappare l'ID in un indice valido dell'array */
static int hash_function(int id, int size) {
    return id % size;
}

int insert_segnalazione_ht(HashTable ht, Segnalazione s) {
    if (!ht || !s) return 0;

    /* Calcolo l'indice del puntatore in cui inserire la segnalazione */
    int index = hash_function(get_id(s), ht->size);

    /* Creo un nuovo nodo per la lista concatenata */
    struct hash_node* nuovo_nodo = (struct hash_node*)malloc(sizeof(struct hash_node));
    if (!nuovo_nodo) return 0;

    /* Inserimento in testa alla lista delle collisioni */
    nuovo_nodo->s = s;
    nuovo_nodo->next = ht->table[index];
    ht->table[index] = nuovo_nodo;

    return 1;
}

Segnalazione search_segnalazione_ht(HashTable ht, int id) {
    if (!ht) return NULL;

    /* Trovo il puntatore corretto */
    int index = hash_function(id, ht->size);
    struct hash_node* corrente = ht->table[index];

    /* Scorro la lista delle collisioni */
    while (corrente != NULL) {
        if (get_id(corrente->s) == id) {
            return corrente->s; /* Segnalazione trovata */
        }
        corrente = corrente->next;
    }

    return NULL; /* Se arrivo qui l'ID non esiste nel sistema */
}

int remove_segnalazione_ht(HashTable ht, int id) {
    if (!ht) return 0;

    int index = hash_function(id, ht->size);
    struct hash_node* corrente = ht->table[index];
    struct hash_node* precedente = NULL;

    /* Scorro la lista per trovare il nodo esatto da eliminare */
    while (corrente != NULL && get_id(corrente->s) != id) {
        precedente = corrente;
        corrente = corrente->next;
    }

    if (corrente == NULL) return 0; /* Nodo non trovato */

    /* * Ricollego i puntatori per escludere il nodo dalla lista
       Caso A: Il nodo da eliminare è il primo della lista 
       Caso B: Il nodo si trova in mezzo o in fondo alla lista */
    if (precedente == NULL) {
        ht->table[index] = corrente->next;
    }
    else {
        precedente->next = corrente->next;
    }

    /* Libero solo il nodo contenitore dell'Hash non la segnalazione. La segnalazione stessa potrebbe servire altrove o essere liberata dal tracker del main */
    free(corrente);
    return 1;
}

void stampa_tutte_ht(HashTable ht) {
    if (!ht) return;
    int trovate = 0;

    /* Scorro tutto l'array della Hash Table */
    for (int i = 0; i < ht->size; i++) {
        struct hash_node* corrente = ht->table[i];
        while (corrente != NULL) {
            stampa_segnalazione(corrente->s);
            trovate++;
            corrente = corrente->next;
        }
    }
    if (trovate == 0) printf("Nessuna segnalazione registrata.\n");
}

void stampa_per_categoria_ht(HashTable ht, const char* categoria) {
    if (!ht) return;
    int trovate = 0;

    for (int i = 0; i < ht->size; i++) {
        struct hash_node* corrente = ht->table[i];
        while (corrente != NULL) {
            /* Uso strcmp per comparare esattamente la stringa della categoria */
            if (strcmp(get_categoria(corrente->s), categoria) == 0) {
                stampa_segnalazione(corrente->s);
                trovate++;
            }
            corrente = corrente->next;
        }
    }
    if (trovate == 0) printf("Nessuna segnalazione per la categoria '%s'.\n", categoria);
}

void stampa_per_stato_ht(HashTable ht, StatoSegnalazione stato) {
    if (!ht) return;
    int trovate = 0;

    for (int i = 0; i < ht->size; i++) {
        struct hash_node* corrente = ht->table[i];
        while (corrente != NULL) {
            if (get_stato(corrente->s) == stato) {
                stampa_segnalazione(corrente->s);
                trovate++;
            }
            corrente = corrente->next;
        }
    }
    if (trovate == 0) printf("Nessuna segnalazione in questo stato.\n");
}

/* lista concatenata creata  per raggruppare e contare le occorrenze delle varie categorie dato che non conosciamo da prima i nomi insariti dell'utente */
struct CatCount {
    char nome[50];
    int conteggio;
    struct CatCount* next;
};

void report_statistiche_ht(HashTable ht) {
    if (!ht) return;

    int totali = 0, aperte = 0, in_lav = 0, chiuse = 0;
    struct CatCount* lista_cat = NULL; /* Testa della lista di conteggio categorie */

    /* visita completa del database per raccogliere i dati */
    for (int i = 0; i < ht->size; i++) {
        struct hash_node* corrente = ht->table[i];
        while (corrente != NULL) {
            totali++;
            StatoSegnalazione st = get_stato(corrente->s);

            if (st == APERTA) aperte++;
            else if (st == IN_LAVORAZIONE) in_lav++;
            else chiuse++;

            /* Gestione frequenza categorie */
            const char* cat_attuale = get_categoria(corrente->s);
            struct CatCount* temp = lista_cat;
            int trovata = 0;

            /* Cerco se la categoria esiste già nella mia lista temporanea */
            while (temp != NULL) {
                if (strcmp(temp->nome, cat_attuale) == 0) {
                    temp->conteggio++;
                    trovata = 1;
                    break; /* Categoria aggiornata, esco dal ciclo */
                }
                temp = temp->next;
            }

            /* Se la categoria è nuova la aggiungo in testa alla lista temporanea */
            if (!trovata) {
                struct CatCount* nuova_cat = (struct CatCount*)malloc(sizeof(struct CatCount));
                if (nuova_cat) {
                    strcpy(nuova_cat->nome, cat_attuale);
                    nuova_cat->conteggio = 1;
                    nuova_cat->next = lista_cat;
                    lista_cat = nuova_cat;
                }
            }
            corrente = corrente->next;
        }
    }

    /* Stampo i risultati e calcolo ilmassimo */
    printf("\n==== REPORT STATISTICHE DETTAGLIATO ====\n");
    printf("Totale Segnalazioni Registrate: %d\n", totali);
    printf(" - Aperte: %d\n", aperte);
    printf(" - In Lavorazione: %d\n", in_lav);
    printf(" - Chiuse: %d\n", chiuse);

    printf("\n--- Segnalazioni per Categoria ---\n");
    struct CatCount* temp = lista_cat;
    char cat_frequente[50] = "Nessuna";
    int max_freq = 0;

    if (temp == NULL) printf("Nessun dato disponibile.\n");

    while (temp != NULL) {
        printf(" - %s: %d\n", temp->nome, temp->conteggio);

        /* Aggiorno la categoria con la frequenza maggiore */
        if (temp->conteggio > max_freq) {
            max_freq = temp->conteggio;
            strcpy(cat_frequente, temp->nome);
        }

        /* Mentre leggo i dati libero la memoria della lista temporanea per evitare sprechi */
        struct CatCount* da_liberare = temp;
        temp = temp->next;
        free(da_liberare);
    }

    printf("\n>>> Categoria piu' frequente: %s (con %d segnalazioni)\n", cat_frequente, max_freq);
    printf("========================================\n");
}

void free_hashtable(HashTable ht) {
    if (!ht) return;

    /* Ciclo per svuotare ogni lista concatenata */
    for (int i = 0; i < ht->size; i++) {
        struct hash_node* corrente = ht->table[i];
        while (corrente != NULL) {
            struct hash_node* temp = corrente;
            corrente = corrente->next;
            free(temp); /* Libero il nodo */
        }
    }

    /* Libero l'array e la struttura stessa */
    free(ht->table);
    free(ht);
}