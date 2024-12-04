/**
 *  File header per la gestione delle funzioni di polling.
 */

#ifndef CLIENT_LIB_WARPPERS_POLLUTILS_H
#define CLIENT_LIB_WARPPERS_POLLUTILS_H

// Funzione per la creazione del file descriptor per la gestione degli eventi di I/O
extern void startEpoll();
// Funzione per l'aggiunta di un file descriptor alla lista di quelli da monitorare
extern void addFileDescriptorToThePolling(int fileDescToWatch);
// Funzione per l'attesa degli eventi dei file descriptor monitorati
extern int waitForEvents();
// Funzione per la rimozione di un file descriptor dalla lista di quelli da monitorare
extern void removeFileDescriptorFromThePolling(int fileDescToRemove);

#endif // CLIENT_LIB_WARPPERS_POLLUTILS_H