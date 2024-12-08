/*
    File header per la gestione di IPC tramite named pipe (FIFO)
*/

#ifndef LIB__WRAPPERS__CUSTOMFIFOTOOLS_H
#define LIB__WRAPPERS__CUSTOMFIFOTOOLS_H

// Creazione della FIFO
extern void createFifo();
// Getter per il file descriptor della FIFO
extern int getFifoFd();
// Impostazione del file descriptor della FIFO
extern void setFifoFd();
// Chiusura della FIFO, cancellazione e reset del file descriptor
extern void deleteFifo();

#endif // LIB__WRAPPERS__CUSTOMFIFOTOOLS_H