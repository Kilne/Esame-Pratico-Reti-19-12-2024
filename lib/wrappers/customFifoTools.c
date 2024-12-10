/*
    File per la gestione di IPC tramite named pipe (FIFO)
    fra il client e il motore di gioco
*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "customErrorPrinting.h"

#define FIFO_PATH "/tmp/gameFifo"
pthread_mutex_t fifoMutex = PTHREAD_MUTEX_INITIALIZER;

// File descriptor per la FIFO
int fifoFd = -1;

/*
    Creazione della FIFO
*/
extern void createFifo()
{
    // Controllo se la FIFO esiste già
    if (access(FIFO_PATH, F_OK) != -1)
    {
        // Ritorno se la FIFO esiste già
        printf("[INFO] La FIFO esiste già\n");
        return;
    }

    // Creazione della FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1)
    {
        customErrorPrinting("[ERROR] mkfifo(): Errore nella creazione della FIFO\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("[INFO] FIFO creata con successo\n");
    }
}

/*
    Getter per il file descriptor della FIFO
*/
extern int getFifoFd()
{
    if (fifoFd == -1)
    {
        customErrorPrinting("[ERROR] getFifoFd(): File descriptor della FIFO non impostato\n");
        exit(EXIT_FAILURE);
    }
    return fifoFd;
}

/*
    Impostazione del file descriptor della FIFO
    in modalità di scrittura e lettura
*/
extern void setFifoFd()
{
    // Apertura della FIFO in lettura e scrittura non bloccante
    fifoFd = open(FIFO_PATH, O_RDWR | O_NONBLOCK);
    if (fifoFd == -1)
    {
        customErrorPrinting("[ERROR] open(): Errore nell'apertura della FIFO\n");
        exit(EXIT_FAILURE);
    }
}
/*
    Cancella la FIFO previa controlli e chiusura del file descriptor
*/
extern void deleteFifo()
{
    // Controllo se la FIFO esiste già
    if (access(FIFO_PATH, F_OK) == -1)
    {
        customErrorPrinting("[ERROR] access(): La FIFO non esiste\n");
    }
    else
    {
        // Chiusura del file descriptor
        if (close(fifoFd) == -1)
        {
            customErrorPrinting("[ERROR] close(): Errore nella chiusura del file descriptor\n");
            exit(EXIT_FAILURE);
        }
        // Rimozione della FIFO
        if (unlink(FIFO_PATH) == -1)
        {
            customErrorPrinting("[ERROR] unlink(): Errore nella rimozione della FIFO\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            // Reset del file descriptor
            fifoFd = -1;
            printf("[INFO] FIFO rimossa con successo\n");
        }
    }
}
/*
    Funzione per la scrittura sulla FIFO in mutua esclusione
    e controllo dell'errore
    @param buffer: buffer da scrivere sulla FIFO
    @return: numero di byte scritti sulla FIFO
*/
extern int customFifoWrite(char *buffer)
{
    // Lock del mutex solo 1 thread alla volta può scrivere sulla FIFO
    pthread_mutex_lock(&fifoMutex);
    int bytesWritten = write(fifoFd, buffer, strlen(buffer) + 1);
    pthread_mutex_unlock(&fifoMutex);

    // Controllo dell'errore
    if (bytesWritten == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // Nessun messaggio da ricevere
            // Continua il ciclo
            return 0;
        }
        else
        {
            customErrorPrinting("[ERROR] write(): Errore nella scrittura sulla FIFO\n");
            exit(EXIT_FAILURE);
        }
    }

    return bytesWritten;
}
/*
    Funzione per la lettura dalla FIFO in mutua esclusione
    e controllo dell'errore
    @param buffer: buffer in cui salvare il messaggio letto dalla FIFO
    @return: numero di byte letti dalla FIFO
*/
extern int customFifoRead(char *buffer)
{
    // Lock del mutex solo 1 thread alla volta può leggere dalla FIFO
    pthread_mutex_lock(&fifoMutex);
    int bytesRead = read(fifoFd, buffer, 20);
    pthread_mutex_unlock(&fifoMutex);

    // Controllo dell'errore
    if (bytesRead == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // Nessun messaggio da ricevere
            // Continua il ciclo
            return 0;
        }
        else
        {
            customErrorPrinting("[ERROR] read(): Errore nella lettura dalla FIFO\n");
            exit(EXIT_FAILURE);
        }
    }

    return bytesRead;
}