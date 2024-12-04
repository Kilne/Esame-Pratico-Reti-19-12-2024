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
#define MAX_EVENTS 100 // Numero massimo di eventi da gestire per file descriptor

// Struttura per la gestione dei file descriptor
struct epoll_event fileDescriptosEventArray[MAX_EVENTS];
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

    @fileDescToWatch: file descriptor da monitorare
*/
extern void addFileDescriptorToThePolling(int fileDescToWatch)
{
    // Controllo se il file descriptor è stato inizializzato, naive ma necessario.
    if (!isEpollInitialized())
    {
        customErrorPrinting("Errore: epoll non inizializzato\n");
        exit(EXIT_FAILURE);
    }

    // Inizializazziione della struttura per la gestione degli eventi provenienti dal file descriptor
    // Eventi da monitorare: EPOLLIN, EPOLLOUT per I/O,  EPOLLRDHUP per i casi in cui il socket venga chiuso
    // lato client(No errore).
    struct epoll_event event;
    event.data.fd = fileDescToWatch;                // File descriptor da monitorare
    event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP; // Eventi da monitorare

    // Aggiunta del file descriptor alla lista di quelli da monitorare
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fileDescToWatch, &event) == -1)
    {
        customErrorPrinting("Errore: epoll_ctl aggiunta fallita\n");
        exit(EXIT_FAILURE);
    }
    // Incremento del contatore interno di file descriptor monitorati
    fileDescriptorCounter++;
}
/*
    Funzione per l'attessa degli eventi di I/O, l'attesa è indefinita.
    @returns: numero di eventi di I/O
*/
extern int waitForEvents()
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

    // Attesa degli eventi di I/O, l'attesa del timeout è indefinita(-1)
    int returnedEvents = epoll_wait(epollFd, fileDescriptosEventArray, MAX_EVENTS, -1);

    return returnedEvents;
}
/*
    Funzione per la rimozione di un file descriptor dalla lista di quelli da monitorare
    dall'istanza di epoll.

    @fileDescToRemove: file descriptor da rimuovere
*/
extern void removeFileDescriptorFromThePolling(int fileDescToRemove)
{
    // Controllo se il file descriptor è stato inizializzato, naive ma necessario.
    if (!isEpollInitialized())
    {
        customErrorPrinting("Errore: epoll non inizializzato\n");
        exit(EXIT_FAILURE);
    }

    // Rimozione del file descriptor dalla lista di quelli da monitorare
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fileDescToRemove, NULL) == -1)
    {
        customErrorPrinting("Errore: epoll_ctl rimozione fallita\n");
        exit(EXIT_FAILURE);
    }

    // Decremento del contatore interno di file descriptor monitorati
    fileDescriptorCounter--;
}