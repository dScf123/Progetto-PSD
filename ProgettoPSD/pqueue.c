#include <stdio.h>
#include <stdlib.h>
#include "pqueue.h"

/* La capacità iniziale dell'array dinamico */
#define INIT_CAPACITY 50

struct pqueue_struct {
    Segnalazione* vet; /* Array di puntatori a segnalazione */
    int numel;         /* Numero di elementi attuali */
    int capacity;      /* Capacità massima dell'array */
};

PQueue crea_pqueue(void) {
    PQueue q = (PQueue)malloc(sizeof(struct pqueue_struct));
    if (q == NULL) return NULL;

    q->numel = 0;
    q->capacity = INIT_CAPACITY;

    /* Aggiunto il cast (size_t) per prevenire l'overflow */
    q->vet = (Segnalazione*)malloc(sizeof(Segnalazione) * (size_t)(q->capacity + 1));

    if (q->vet == NULL) {
        free(q);
        return NULL;
    }
    return q;
}

int is_empty_pq(PQueue q) {
    if (q == NULL) return 1;
    return q->numel == 0;
}

/* Funzione interna per far "salire" un elemento e mantenere la proprietà del Max-Heap */
static void sali(PQueue q) {
    int i = q->numel;
    Segnalazione temp;

    /* Confrontiamo l'urgenza dell'elemento i con suo padre (i/2) */
    while (i > 1 && get_urgenza(q->vet[i]) > get_urgenza(q->vet[i / 2])) {
        /* Scambia */
        temp = q->vet[i];
        q->vet[i] = q->vet[i / 2];
        q->vet[i / 2] = temp;
        i = i / 2;
    }
}

int insert_pq(PQueue q, Segnalazione s) {
    if (q == NULL || s == NULL) return 0;

    if (q->numel == q->capacity) {
        int nuova_capacita = q->capacity * 2;

        /* Aggiunto il cast (size_t) per prevenire l'overflow */
        Segnalazione* temp_vet = (Segnalazione*)realloc(q->vet, sizeof(Segnalazione) * (size_t)(nuova_capacita + 1));

        if (temp_vet == NULL) {
            return 0;
        }

        q->vet = temp_vet;
        q->capacity = nuova_capacita;
    }

    q->numel++;
    q->vet[q->numel] = s;
    sali(q);

    return 1;
}

/* Funzione interna per far "scendere" la nuova radice dopo l'estrazione */
static void scendi(PQueue q) {
    int i = 1, pos;
    Segnalazione temp;
    int n = q->numel;

    while (1) {
        if (2 * i + 1 <= n) {
            /* Ha due figli: trovo il maggiore tra i due */
            pos = (get_urgenza(q->vet[i * 2]) > get_urgenza(q->vet[i * 2 + 1])) ? i * 2 : i * 2 + 1;
        }
        else if (2 * i <= n) {
            /* Ha solo il figlio sinistro */
            pos = 2 * i;
        }
        else {
            /* Non ha figli, mi fermo */
            break;
        }

        /* Se il figlio maggiore è più urgente del padre, li scambia */
        if (get_urgenza(q->vet[pos]) > get_urgenza(q->vet[i])) {
            temp = q->vet[i];
            q->vet[i] = q->vet[pos];
            q->vet[pos] = temp;
            i = pos;
        }
        else {
            break; /* È già al posto giusto */
        }
    }
}

Segnalazione get_max_pq(PQueue q) {
    if (is_empty_pq(q)) return NULL;
    return q->vet[1];
}

Segnalazione extract_max_pq(PQueue q) {
    if (is_empty_pq(q)) return NULL;

    Segnalazione max = q->vet[1]; /* La segnalazione più urgente è in cima */
    q->vet[1] = q->vet[q->numel]; /* Sposto l'ultima foglia in cima */
    q->numel--;
    scendi(q); /* Riaggiusto lo Heap */

    return max;
}

void free_pqueue(PQueue q) {
    if (q != NULL) {
        free(q->vet);
        free(q);
    }
}