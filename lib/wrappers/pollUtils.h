/**
 *  File header per la gestione delle funzioni di polling.
 */

#ifndef CLIENT_LIB_WARPPERS_POLLUTILS_H
#define CLIENT_LIB_WARPPERS_POLLUTILS_H

#include <sys/epoll.h>

// Funzione per la creazione del file descriptor per la gestione degli eventi di I/O
extern void startEpoll();
// Funzione per l'aggiunta di un file descriptor alla lista di quelli da monitorare
extern void addFileDescriptorToThePolling(int fileDescToWatch, uint32_t eventFlags);
// Funzione per l'attesa degli eventi dei file descriptor monitorati
extern int waitForEvents(struct epoll_event *eventsArray, int MAX_EVENTS);
// Funzione per la rimozione di un file descriptor dalla lista di quelli da monitorare
extern void removeFileDescriptorFromThePolling(int fileDescToRemove);
// Funzione per la modifica dei flag di un file descriptor
extern void modifyFileDescFlags(int fdToMod, uint32_t newFlag);
// Chiusura del file descriptor per la gestione degli eventi di I/O
extern void closeEpoll();

#endif // CLIENT_LIB_WARPPERS_POLLUTILS_H