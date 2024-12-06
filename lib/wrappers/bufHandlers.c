/*
    File per la gestione dei buffer di messaggi UDP
*/
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

/*
    Funzione per la creazione di un messaggio UDP di default 512 byte
    @return char * messaggio UDP di default
*/
extern char *getStdUDPMessage()
{
    // Alloco la memoria per il messaggio UDP massimo di 512 byte
    char *message = calloc(512, sizeof(char));
    // Controllo se l'allocazione è andata a buon fine
    if (message == NULL)
    {
        perror("Errore nell'allocazione della memoria per il messaggio UDP");
        exit(EXIT_FAILURE);
    }
    else
    {
        return message;
    }
}
/*
    Funzione per la deallocazione della memoria per il messaggio UDP
    @param char * messaggio UDP da deallocare
*/
extern void freeUDPMessage(char *message)
{
    // Libero la memoria allocata per il messaggio UDP ed assegno NULL al puntatore.
    free(message);
    message = NULL;
    // Controllo se la deallocazione è andata a buon fine
    if (message != NULL)
    {
        fprintf(stderr, "[ERROR] free(): Errore nella deallocazione della memoria per il messaggio UDP\n");
        exit(EXIT_FAILURE);
    }
}
/*
    Funzione per accomodare un messaggio UDP in un buffer di dimensione standard
    @param char ** bugger: buffer in cui inserire il messaggio
    @param char * message: messaggio da inserire nel buffer
    @param size_t m_len: lunghezza del messaggio
*/
extern void setUDPMessage(char **buffer, char *message, size_t m_len)
{
    // Controllo eccessiva lunghezza del nuovo buffer
    if (m_len > 512)
    {
        fprintf(stderr,"[ERRORE] Il messaggio eccede la grandezza impostata per il pacchetto UDP(512)\n");
        exit(EXIT_FAILURE);
    }
    // Nuova locazione di memoria memorizzata temporaneamente
    char *tempPtr = realloc(*buffer, m_len);
    // Copia contenuti con memmove per evitare socrascritture contigue
    memmove(tempPtr, message, m_len);
    //Assegnazione puntatore della nuova locazione al buffer originale
    *buffer = tempPtr;
}