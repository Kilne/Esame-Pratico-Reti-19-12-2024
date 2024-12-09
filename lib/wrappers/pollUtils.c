/*
    File contenete le poll utility per la gestione dei client, del STDERR/STDIO lato server e invio e datagrammi

    Le funzioni utilizzano le moderne chiamate di sistema epoll_create1, epoll_ctl e epoll_wait per la gestione
    degli eventi di I/O. Anzichè utilizzare le vecchie chiamate di sistema poll e select.
*/

#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "customErrorPrinting.h"

// Epoll file descriptor
int epollFd = -1;
// Contatore interno di file descriptor monitorati
int fileDescriptorCounter = 0;

/*
    Funzione per la verifica dell'inizializzazione dell'istanza di epoll
    @return: 1 se l'istanza è stata inizializzata, 0 altrimenti
*/
int isEpollInitialized()
{
    return epollFd != -1;
}

/*
 * Funzione per la creazione del file descriptor per la gestione degli eventi di I/O
 */
extern void startEpoll()
{
    // Creazione del file descriptor per la gestione degli eventi di I/O
    // Utilizzo di epoll_create1 per la creazione del file descriptor
    // Il suo primo argomento è 0(flags) che la rende una chiamata simile a epoll_create
    // il cui argomento size è obsoleto.
    epollFd = epoll_create1(0);
    if (epollFd == -1)
    {
        customErrorPrinting("Errore: epoll_create1 fallita\n");
        exit(EXIT_FAILURE);
    }
}
/*
    Funzione per l'aggiunta di un file descriptor alla lista di quelli da monitorare
    dall'istanza di epoll.

    @param fileDescToWatch: file descriptor da monitorare
    @param eventFlags: flag per la gestione degli eventi di I/O
*/
extern void addFileDescriptorToThePolling(int fileDescToWatch, uint32_t eventFlags)
{
    // Controllo se il file descriptor è stato inizializzato, naive ma necessario.
    if (!isEpollInitialized())
    {
        customErrorPrinting("Errore: epoll non inizializzato\n");
        exit(EXIT_FAILURE);
    }

    // Inizializzazione della struttura per la gestione degli eventi
    struct epoll_event eventType;
    eventType.data.fd = fileDescToWatch; // File descriptor da monitorare
    eventType.events = eventFlags;       // Eventi da monitorare

    // Aggiunta del file descriptor alla lista di quelli da monitorare
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fileDescToWatch, &eventType) == -1)
    {
        customErrorPrinting("Errore: epoll_ctl aggiunta FD fallita\n");
        exit(EXIT_FAILURE);
    }
    // Incremento del contatore interno di file descriptor monitorati
    fileDescriptorCounter++;
}
/*
    Funzione per l'attessa degli eventi di I/O, l'attesa è indefinita.
    @eventsArray: array di eventi di I/O
    @MaxEvents: numero massimo di eventi di I/O
    @returns: numero di eventi di I/O
*/
extern int waitForEvents(struct epoll_event *eventsArray, int MAX_EVENTS)
{
    // Controllo se Epoll è stato inizializzato
    if (!isEpollInitialized())
    {
        customErrorPrinting("Errore: epoll non inizializzato\n");
        exit(EXIT_FAILURE);
    }
    // Controllo se ci sono file descriptor da monitorare
    if (fileDescriptorCounter == 0)
    {
        customErrorPrinting("Errore: nessun file descriptor da monitorare\n");
        exit(EXIT_FAILURE);
    }

    // Attesa degli eventi di I/O
    int returnedEvents = epoll_wait(epollFd, eventsArray, MAX_EVENTS, -1);
    if (returnedEvents == -1)
    {
        customErrorPrinting("[ERROR] Errore nell'attesa degli eventi\n");
        exit(EXIT_FAILURE);
    }

    return returnedEvents;
}
/*
    Funzione per la rimozione di un file descriptor dalla lista di quelli da monitorare
    dall'istanza di epoll.

    @fileDescToRemove: file descriptor da rimuovere
*/
extern void removeFileDescriptorFromThePolling(int fileDescToRemove)
{
    // Controllo se l'istanza di epoll è stata inizializzata.
    if (!isEpollInitialized())
    {
        customErrorPrinting("Errore: epoll non inizializzato\n");
        exit(EXIT_FAILURE);
    }

    // Rimozione del file descriptor dalla lista di quelli da monitorare
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fileDescToRemove, NULL) == -1)
    {
        customErrorPrinting("Errore: epoll_ctl rimozione FD fallita\n");
        exit(EXIT_FAILURE);
    }

    // Decremento del contatore interno di file descriptor monitorati
    fileDescriptorCounter--;
}
/*
    Funzione per la modifica delle flag di un file descriptor.
    @param fdToMod: file descriptor da modificare
    @param newFlag: nuova flag da assegnare al file descriptor

*/
extern void modifyFileDescFlags(int fdToMod, uint32_t newFlag)
{
    // Modifica delle flag del file descriptor
    struct epoll_event eventType;
    eventType.data.fd = fdToMod;
    eventType.events = newFlag;
    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fdToMod, &eventType) == -1)
    {
        customErrorPrinting("[ERROR] epoll_ctl(): modifica flag FD fallita\n");
        exit(EXIT_FAILURE);
    }
}
/*
    Funzione per la chiusura dell'istanza di epoll
*/
extern void closeEpoll()
{
    // Controllo se l'istanza di epoll è stata inizializzata
    if (!isEpollInitialized())
    {
        customErrorPrinting("Errore: epoll non inizializzato\n");
        exit(EXIT_FAILURE);
    }

    // Chiusura dell'istanza di epoll
    if (close(epollFd) == -1)
    {
        customErrorPrinting("Errore: chiusura epoll fallita\n");
        exit(EXIT_FAILURE);
    }
}