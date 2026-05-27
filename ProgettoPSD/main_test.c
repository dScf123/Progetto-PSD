#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> /* Libreria per i test automatici */
#include "segnalazione.h"
#include "hash.h"
#include "pqueue.h"

/* Utilizza la macro assert() per verificare che i risultati delle funzioni corrispondano a quelli attesi. Se un assert fallisce, il programma si blocca, dimostrando l'esatta riga in cui l'algoritmo ha un difetto */
int main(void) {
    printf(" INIZIO TEST AUTOMATICI \n\n");

    /* TEST 1: Creazione e manipolazione ADT Segnalazione */
    printf("Test 1: Verifica ADT Segnalazione... ");
    Segnalazione s1 = crea_segnalazione(100, "Mario Rossi", "Buche", "Buca in centro", "10/05/2026", 5);
    Segnalazione s2 = crea_segnalazione(101, "Luigi Verdi", "Illuminazione", "Lampione rotto", "11/05/2026", 2);

    assert(s1 != NULL && s2 != NULL); /* Verifica allocazione avvenuta */
    assert(get_id(s1) == 100);        /* Verifica getter */
    assert(get_urgenza(s2) == 2);
    assert(get_stato(s1) == APERTA);  /* Lo stato iniziale deve essere APERTA */
    printf("PASSATO!\n");

    /* TEST 2: Inserimento e Ricerca in HashTable */
    printf("Test 2: Verifica ADT HashTable (Inserimento e Ricerca)... ");
    HashTable ht = crea_hashtable(11);
    assert(ht != NULL);

    insert_segnalazione_ht(ht, s1);
    insert_segnalazione_ht(ht, s2);

    Segnalazione trovata = search_segnalazione_ht(ht, 100);
    assert(trovata != NULL);
    assert(strcmp(get_categoria(trovata), "Buche") == 0); /* Verifica correttezza dato recuperato */

    Segnalazione non_trovata = search_segnalazione_ht(ht, 999);
    assert(non_trovata == NULL); /* ID inesistente deve ritornare NULL */
    printf("PASSATO!\n");

    /* TEST 3: Ordinamento e Max-Heap in Priority Queue */
    printf("Test 3: Verifica ADT Priority Queue (Max-Heap)... ");
    PQueue pq = crea_pqueue();
    assert(pq != NULL);

    /* Inserisco prima l'urgenza bassa (2) e poi l'urgenza alta (5) */
    insert_pq(pq, s2);
    insert_pq(pq, s1);

    /* L'estrazione DEVE restituirmi s1 (urgenza 5), anche se inserita per ultima */
    Segnalazione piu_urgente = extract_max_pq(pq);
    assert(piu_urgente != NULL);
    assert(get_id(piu_urgente) == 100); /* Il Max-Heap ha riordinato correttamente */
    printf("PASSATO!\n");

    /* TEST 4: Modifica stato e Lazy Deletion logica */
    printf("Test 4: Verifica Aggiornamento e Rimozione Logica... ");
    set_stato(s1, IN_LAVORAZIONE);
    assert(get_stato(s1) == IN_LAVORAZIONE);

    int rimosso = remove_segnalazione_ht(ht, 101);
    assert(rimosso == 1); /* Rimozione dalla tabella hash andata a buon fine */
    assert(search_segnalazione_ht(ht, 101) == NULL); /* Ora non deve esistere piu' */
    printf("PASSATO\n");

    /* TEST 5: Verifica della visualizzazione filtrata */
    printf(" TEST 5: Verifica visualizzazione filtrata \n");
    printf(" Filtro per Categoria 'Buche' (Attesi ID 100 e 102) \n");
    stampa_per_categoria_ht(ht, "Buche");

    printf(" Filtro per Stato 'APERTA' (Attesi tutti e 3 gli ID) \n");
    stampa_per_stato_ht(ht, APERTA);

    /* TEST 6: Test della generazione del report finale */
    printf("\n TEST 6: Verifica report finale \n");
    report_statistiche_ht(ht);
    
    /* Pulizia memoria per i test */
    free_hashtable(ht);
    free_pqueue(pq);
    free_segnalazione(s1);
    free_segnalazione(s2);

    printf("\n TUTTI I TEST SUPERATI CON SUCCESSO \n");
    return 0;
}