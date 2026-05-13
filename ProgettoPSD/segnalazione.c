#define _CRT_SECURE_NO_WARNINGS /* Per poter usare strcpy */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "segnalazione.h"

struct segnalazione_struct {
    int id;
    char* nome;
    char* categoria;
    char* descrizione;
    char* data;
    int urgenza;       /* Da 1 a 5, utile per la nostra Priority Queue */
    StatoSegnalazione stato;
};

Segnalazione crea_segnalazione(int id, const char* nome, const char* categoria, const char* descrizione, const char* data, int urgenza) {
    Segnalazione s = (Segnalazione)malloc(sizeof(struct segnalazione_struct));
    if (s == NULL) return NULL;

    s->id = id;
    s->urgenza = urgenza;
    s->stato = APERTA; /* Una nuova segnalazione nasce sempre APERTA */

    /* Allocazione dinamica per le stringhe  */
    s->nome = (char*)malloc(strlen(nome) + 1);
    strcpy(s->nome, nome);

    s->categoria = (char*)malloc(strlen(categoria) + 1);
    strcpy(s->categoria, categoria);

    s->descrizione = (char*)malloc(strlen(descrizione) + 1);
    strcpy(s->descrizione, descrizione);

    s->data = (char*)malloc(strlen(data) + 1);
    strcpy(s->data, data);

    return s;
}

int get_id(Segnalazione s) { return s->id; }
const char* get_categoria(Segnalazione s) { return s->categoria; }
int get_urgenza(Segnalazione s) { return s->urgenza; }
StatoSegnalazione get_stato(Segnalazione s) { return s->stato; }
void set_stato(Segnalazione s, StatoSegnalazione nuovo_stato) { s->stato = nuovo_stato; }

void stampa_segnalazione(Segnalazione s) {
    if (!s) return;
    const char* stati_str[] = { "APERTA", "IN LAVORAZIONE", "CHIUSA" };

    printf("\n[ID: %d] Categoria: %s - Data: %s\n", s->id, s->categoria, s->data);
    printf("Cittadino: %s\n", s->nome);
    printf("Descrizione: %s\n", s->descrizione);
    printf("Urgenza: %d/5 | Stato: %s\n", s->urgenza, stati_str[s->stato]);
}

void free_segnalazione(Segnalazione s) {
    if (s != NULL) {
        free(s->nome);
        free(s->categoria);
        free(s->descrizione);
        free(s->data);
        free(s);
    }
}