/*
    Motore di gioco
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lib/wrappers/pollUtils.h"
#include "lib/gamelogics/drawingField.h"
#include "lib/wrappers/customFifoTools.h"

int main()
{
    // Chiusura della FIFO se esiste
    atexit(deleteFifo);

    // Inizializzazione della griglia di gioco
    init();

    // Creazione della FIFO
    createFifo();
    setFifoFd();
    int fifoFd = getFifoFd();

    // Inizializzazione del buffer per la lettura della FIFO
    // per la riga di meteoriti
    char *fifoBuffer = calloc(20, sizeof(char));

    // Inzializzazione Epoll, monitoraggio della FIFO
    startEpoll();
    addFileDescriptorToThePolling(fifoFd, EPOLLIN); // TODO: solo EPOOLLIN??

    // Inizializzazione del monitoraggio dell'input da tastiera
    setTerminalMode(1); // Modalità raw
    addFileDescriptorToThePolling(STDIN_FILENO, EPOLLIN);

    // Ciclo di gioco
    while (1)
    {
        // Ad ogni ciclo ridisegno la griglia di gioco
        printGrid();

        // Eventi FIFO
        struct epoll_event events[2]; // FIFO e tastiera

        // Attesa degli eventi di I/O
        int triggeredEvents = waitForEvents(events, 2);

        // Gestione degli eventi
        if (triggeredEvents > 0)
        {
            // Controllo evento
            for (int i = 0; i < triggeredEvents; i++)
            {
                if (events[i].data.fd == fifoFd)
                {
                    // Lettura dalla FIFO
                    read(fifoFd, fifoBuffer, 20);
                    addMeteors(fifoBuffer);
                }
                else if (events[i].data.fd == STDIN_FILENO)
                {
                    // Lettura da tastiera
                    char c[10];
                    int n = read(STDIN_FILENO, c, 9);
                    checkTheInput(c, n);
                }
            }
        }
    }

    return 0;
}