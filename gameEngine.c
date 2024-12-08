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
        // Tick rate per ogni refresh della griglia e dei segnali per la Epoll
        sleep(1);
        // Ad ogni ciclo ridisegno la griglia di gioco
        printGrid();

        // Eventi FIFO
        struct epoll_event events[2]; // FIFO e tastiera
        /*
            - L'Epoll sta bloccando il processo fino a che non si verifica un evento, non va bene per il gioco per i client / server
            potrebbe andare bene, analizzare il caso d'uso e decidere.
            - L'immissione di un carattere non viene presa dallo STDIN_FILENO, senza che si prema invio rischiando di aggiungere
            un carattere di troppo
            - Mettendo un carattere terminatore in CheckTheInput al confronto successivo con strcmp non viene MAI trovata
            la corrispondenza con le macro e carattere di uscita, quindi non si esce mai dal gioco ne muove la nave.

        */
        int triggeredEvents = waitForEvents(events, 2);

        // Gestione degli eventi
        if (triggeredEvents > 0)
        {
            // Controllo evento
            if (events[0].data.fd == fifoFd)
            {
                // Lettura dalla FIFO
                read(fifoFd, fifoBuffer, 20);
                addMeteors(fifoBuffer);
            }
            else if (events[0].data.fd == STDIN_FILENO)
            {
                // Lettura da tastiera
                char c[10];
                int n = read(STDIN_FILENO, c, 9);
                checkTheInput(c, n);
            }
        }
    }

    return 0;
}