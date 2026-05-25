#define _CRT_SECURE_NO_WARNINGS /* Per poter usare strcpy */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "segnalazione.h"
#include "hash.h"
#include "pqueue.h"

/* Questo nodo memorizza unicamente il puntatore a ogni segnalazione creata e serve esclusivamente a garantire la deallocazione sicura a fine programma */
typedef struct mem_node {
    Segnalazione s;
    struct mem_node* next;
} MemNode;

/* Aggiunge una segnalazione in testa alla lista del tracker restituisce la nuova testa della lista tracker (resa statica per risparmiare memoria ed evitiare conflitti di nomi con altri file) */
static MemNode* aggiungi_a_tracker(MemNode* head, Segnalazione s) {
    MemNode* nuovo = (MemNode*)malloc(sizeof(MemNode));
    if (nuovo) {
        nuovo->s = s;
        nuovo->next = head;
        head = nuovo;
    }
    return head;
}

/* Scorre l'intera lista del tracker e dealloca fisicamente la memoria di ogni segnalazione (resa statica per risparmiare memoria ed evitiare conflitti di nomi con altri file) */
static void libera_tutta_la_memoria(MemNode* head) {
    while (head != NULL) {
        MemNode* temp = head;
        head = head->next;
        /* Rilascia in modo sicuro le stringhe dinamiche e la struct Segnalazione */
        free_segnalazione(temp->s);
        free(temp); /* Libera il nodo del tracker stesso */
    }
}

/* Serve per svuotare il buffer dello standard input (stdin) previene bug di lettura quando si alternano scanf() e fgets(), (resa statica per risparmiare memoria ed evitiare conflitti di nomi con altri file) */
static void pulisci_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(void) {
    /* Inizializzazione delle strutture dati principali */
    HashTable ht = crea_hashtable(101); /* 101 e' un numero primo e serve per ridurre le collisioni */
    PQueue pq = crea_pqueue();
    MemNode* tracker = NULL; /* Inizializzazione del gestore di memoria privato */

    int scelta;
    int id_counter = 1; /* Generatore dei codici identificativi */

    printf("==================================================================\n");
    printf("     SISTEMA GESTIONE SEGNALAZIONI URBAN-CARE - COMUNE \n");
    printf("==================================================================\n");

    do {
        printf("\n--- MENU PRINCIPALE ---\n");
        printf("1. Inserisci nuova segnalazione\n");
        printf("2. Visualizza TUTTE le segnalazioni\n");
        printf("3. Cerca segnalazione (per ID)\n");
        printf("4. Cerca segnalazioni (per Categoria)\n");
        printf("5. Visualizza segnalazioni per STATO (Aperte/In Lavorazione/Chiuse)\n");
        printf("6. Aggiorna stato segnalazione\n");
        printf("7. Estrai segnalazione PIU' URGENTE (da elaborare)\n");
        printf("8. Elimina segnalazione\n");
        printf("9. Genera Report Statistico Completo\n");
        printf("0. Esci dal sistema\n");
        printf("Scelta: ");

        /* Controllo di validita' dell'input per evitare loop infiniti se si inseriscono lettere */
        if (scanf("%d", &scelta) != 1) {
            pulisci_buffer();
            scelta = -1;
        }
        else {
            pulisci_buffer();
        }

        switch (scelta) {
        case 1: {
            char nome[50], categoria[50], descr[150], data[20];
            int urgenza;

            /* Lettura sicura tramite fgets e rimozione del carattere '\n' finale */
            printf("Nome cittadino: "); fgets(nome, 50, stdin); nome[strcspn(nome, "\n")] = 0;
            printf("Categoria (es. Buche, Illuminazione, Rifiuti): "); fgets(categoria, 50, stdin); categoria[strcspn(categoria, "\n")] = 0;
            printf("Descrizione problema: "); fgets(descr, 150, stdin); descr[strcspn(descr, "\n")] = 0;
            printf("Data (GG/MM/AAAA): "); fgets(data, 20, stdin); data[strcspn(data, "\n")] = 0;

            printf("Livello urgenza (1=Minima ... 5=Massima): ");
            /* In caso un utente non inserisce un numero */
            if (scanf("%d", &urgenza) != 1) urgenza = 1;
            pulisci_buffer();

            /* Limito l'urgenza all'interno dei limiti stabiliti */
            if (urgenza < 1) urgenza = 1;
            if (urgenza > 5) urgenza = 5;

            /* Allocazione dinamica del record della segnalazione */
            Segnalazione s = crea_segnalazione(id_counter++, nome, categoria, descr, data, urgenza);
            if (s) {
                tracker = aggiungi_a_tracker(tracker, s); /* Inserisco nel tracker per la free finale */
                insert_segnalazione_ht(ht, s);            /* Riferimento inserito nel Database */
                insert_pq(pq, s);                         /* Riferimento inserito nella Coda a Priorita */
                printf(">>> Segnalazione salvata con successo! (ID Assegnato: %d)\n", id_counter - 1);
            }
            else {
                printf(">>> Errore critico: Allocazione memoria fallita.\n");
            }
            break;
        }
        case 2:
            printf("\n--- TUTTE LE SEGNALAZIONI NEL DATABASE ---\n");
            stampa_tutte_ht(ht);
            break;

        case 3: {
            int id_ricerca;
            printf("Inserisci ID da cercare: "); 
            if (scanf("%d", &id_ricerca) != 1) id_ricerca = -1;
            pulisci_buffer();
            Segnalazione trovata = search_segnalazione_ht(ht, id_ricerca);
            if (trovata) stampa_segnalazione(trovata);
            else printf(">>> Nessuna segnalazione trovata con ID %d.\n", id_ricerca);
            break;
        }
        case 4: {
            char cat_ricerca[50];
            printf("Inserisci Categoria da cercare: "); fgets(cat_ricerca, 50, stdin); cat_ricerca[strcspn(cat_ricerca, "\n")] = 0;
            printf("\n--- SEGNALAZIONI PER CATEGORIA: %s ---\n", cat_ricerca);
            stampa_per_categoria_ht(ht, cat_ricerca);
            break;
        }
        case 5: {
            int st_ricerca;
            printf("Seleziona lo stato da filtrare (0=APERTA, 1=IN LAVORAZIONE, 2=CHIUSA): ");
            if (scanf("%d", &st_ricerca) != 1) st_ricerca = -1;
            pulisci_buffer();
            if (st_ricerca >= 0 && st_ricerca <= 2) {
                printf("\n--- SEGNALAZIONI FILTRATE PER STATO ---\n");
                stampa_per_stato_ht(ht, (StatoSegnalazione)st_ricerca);
            }
            else {
                printf(">>> Scelta non valida.\n");
            }
            break;
        }
        case 6: {
            int id_agg, st;
            printf("Inserisci ID della segnalazione da aggiornare: "); 
            if (scanf("%d", &id_agg) != 1) id_agg = -1;
            pulisci_buffer();
            Segnalazione s = search_segnalazione_ht(ht, id_agg);
            if (s) {
                printf("Stato attuale: %d (0=APERTA, 1=IN_LAVORAZIONE, 2=CHIUSA)\n", get_stato(s));
                printf("Nuovo stato (0, 1, 2): ");
                if (scanf("%d", &st) != 1) st = -1;
                pulisci_buffer();
                if (st >= 0 && st <= 2) {
                    set_stato(s, (StatoSegnalazione)st);
                    printf(">>> Stato aggiornato con successo!\n");
                }
                else printf(">>> Stato non valido.\n");
            }
            else printf(">>> Segnalazione non trovata.\n");
            break;
        }
        case 7: {
            Segnalazione urgente = NULL;
            int trovata_valida = 0;

            /* Quando eliminiamo una segnalazione, la togliamo solo dalla Tabella Hash. Nell'Heap la lasciamo lì, come una sorta di "fantasma", finché non arriva naturalmente in cima ed è il suo turno di essere estratta */
            while (!is_empty_pq(pq) && !trovata_valida) {
                urgente = extract_max_pq(pq);
                if (urgente && search_segnalazione_ht(ht, get_id(urgente)) != NULL && get_stato(urgente) != CHIUSA) {
                    trovata_valida = 1;
                }
            }

            if (trovata_valida) {
                printf("\n--- SEGNALAZIONE PIU' URGENTE PRELEVATA DALLA CODA ---\n");
                stampa_segnalazione(urgente);
                printf(">>> Spostata in stato 'IN LAVORAZIONE' in automatico.\n");
                set_stato(urgente, IN_LAVORAZIONE);
            }
            else {
                printf(">>> Nessuna segnalazione urgente in attesa di elaborazione.\n");
            }
            break;
        }
        case 8: {
            int id_canc;
            printf("Inserisci ID della segnalazione da eliminare: ");
            if (scanf("%d", &id_canc) != 1) id_canc = -1;
            pulisci_buffer();
            /* Rimozione dalla Tabelle Hash.La deallocazione fisica avverra solo a fine programma */
            if (remove_segnalazione_ht(ht, id_canc)) {
                printf(">>> Segnalazione rimossa dal Database operativo.\n");
            }
            else {
                printf(">>> Segnalazione non trovata o gia' eliminata.\n");
            }
            break;
        }
        case 9:
            /* Genera il report delle statistiche calcolando anche le frequenze delle categorie */
            report_statistiche_ht(ht);
            break;

        case 0:
            printf("Chiusura del sistema in corso...\n");
            break;

        default:
            printf(">>> Scelta non valida. Riprova.\n");
        }
    } while (scelta != 0);

    /* Deallocazione della memoria dinamica */
    free_hashtable(ht); /* Libera i nodi e i puntatori del dizionario */
    free_pqueue(pq);    /* Libera il vettore dinamico della coda a priorita' */
    libera_tutta_la_memoria(tracker); /* Libera fisicamente le stringhe dinamiche e le struct Segnalazione */

    printf("Tutta la memoria e' stata rilasciata correttamente. Arrivederci!\n");
    return 0;
}