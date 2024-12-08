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
#include "customErrorPrinting.h"

#define FIFO_PATH "/tmp/gameFifo"

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
        // Rimozione della FIFO
        if (unlink(FIFO_PATH) == -1)
        {
            customErrorPrinting("[ERROR] unlink(): Errore nella rimozione della FIFO\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("[INFO] FIFO rimossa con successo\n");
        }
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
    // Apertura della FIFO in lettura e scrittura
    fifoFd = open(FIFO_PATH, O_RDWR | O_NONBLOCK);
    if (fifoFd == -1)
    {
        customErrorPrinting("[ERROR] open(): Errore nell'apertura della FIFO\n");
        exit(EXIT_FAILURE);
    }
}