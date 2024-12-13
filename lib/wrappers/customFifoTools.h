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
// Scrittura sulla FIFO in mutua esclusione
extern int customFifoWrite(char *buffer);
// Lettura dalla FIFO in mutua esclusione
extern int customFifoRead(char *buffer);

#endif // LIB__WRAPPERS__CUSTOMFIFOTOOLS_H