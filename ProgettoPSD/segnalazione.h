#ifndef SEGNALAZIONE_H
#define SEGNALAZIONE_H

/* Definizione degli stati possibili per una segnalazione */
typedef enum { APERTA, IN_LAVORAZIONE, CHIUSA } StatoSegnalazione;

/* Puntatore opaco */
typedef struct segnalazione_struct* Segnalazione;

/* Operatori per creare e gestire la segnalazione */
Segnalazione crea_segnalazione(int id, const char* nome, const char* categoria, const char* descrizione, const char* data, int urgenza);
int get_id(Segnalazione s);
const char* get_categoria(Segnalazione s);
int get_urgenza(Segnalazione s);
StatoSegnalazione get_stato(Segnalazione s);
void set_stato(Segnalazione s, StatoSegnalazione nuovo_stato);
void stampa_segnalazione(Segnalazione s);
void free_segnalazione(Segnalazione s);

#endif
