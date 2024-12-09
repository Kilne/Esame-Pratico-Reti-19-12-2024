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

    // Creazione della FIFO per il client
    createFifo();
    setFifoFd();
    int fifoFd = getFifoFd();

    // Inizializzazione del buffer per la lettura della FIFO del client
    // per la riga di meteoriti
    char *fifoBuffer = calloc(20, sizeof(char));

    // Inzializzazione Epoll, monitoraggio delle FIFO e dell'input da tastiera
    startEpoll();
    addFileDescriptorToThePolling(fifoFd, EPOLLIN); // TODO: solo EPOOLLIN??
    addFileDescriptorToThePolling(STDIN_FILENO, EPOLLIN);

    // Inizializzazione del monitoraggio dell'input da tastiera
    setTerminalMode(1); // Modalità raw

    /*
        TODO:
            - Implementare la gestione del game over con eventuali FIFO tra game engine e drawer,
            passare poi il messaggio al client per la terminazione del gioco.
    */

    // Ciclo di gioco
    while (1)
    {
        // Ad ogni ciclo ridisegno la griglia di gioco
        printGrid();

        // Eventi per FIFO e tastiera
        struct epoll_event events[2];

        // Attesa degli eventi di I/O
        int triggeredEvents = waitForEvents(events, 2);

        // Gestione degli eventi
        if (triggeredEvents > 0)
        {
            // Controllo evento
            for (int i = 0; i < triggeredEvents; i++)
            {
                // Controllo se l'evento è relativo alla FIFO del client
                if (events[i].data.fd == fifoFd)
                {
                    // Lettura dalla FIFO
                    int fifoRead = read(fifoFd, fifoBuffer, 20);
                    printf("[INFO] Letti %d byte dalla FIFO\n", fifoRead);
                    // Aggiunta dei meteoriti alla griglia di gioco
                    addMeteors(fifoBuffer);
                }
                else if (events[i].data.fd == STDIN_FILENO) // Controllo se l'evento è relativo all'input da tastiera
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