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
#include "lib/wrappers/customErrorPrinting.h"

int main()
{

    // Inizializzazione della griglia di gioco
    init();

    // Acquisizione del file descriptor della FIFO
    createFifo();
    setFifoFd();
    int fifoFd = getFifoFd();

    // Inizializzazione del buffer per la lettura della FIFO del client
    // per la riga di meteoriti
    char *fifoBuffer = calloc(20, sizeof(char));

    // Inzializzazione Epoll, monitoraggio delle FIFO e dell'input da tastiera
    startEpoll();
    addFileDescriptorToThePolling(fifoFd, EPOLLIN);
    addFileDescriptorToThePolling(STDIN_FILENO, EPOLLIN);
    printf("[INFO] Epoll inizializzato\n");

    // Inizializzazione del monitoraggio dell'input da tastiera
    setTerminalMode(1); // Modalità raw

    /*
        TODO:
            - Implementare la gestione del game over con eventuali FIFO tra game engine e drawer,
            passare poi il messaggio al client per la terminazione del gioco.
    */
    // Controllo game over
    int gameOver = 0;                  // 0: no game over, 1: game over
    int quittingGame = 0;              // 0: non si sta uscendo dal gioco, 1: si sta uscendo dal gioco
    char *gameOverString = "GAMEOVER"; // Stringa per il client

    // Ciclo di gioco
    while (gameOver == 0)
    {
        // Ad ogni ciclo ridisegno la griglia di gioco
        printGrid();

        // Eventi per FIFO e tastiera
        struct epoll_event events[150];

        // Attesa degli eventi di I/O
        int triggeredEvents = waitForEvents(events, 150);

        // Gestione degli eventi
        if (triggeredEvents > 0)
        {
            // Controllo evento
            for (int i = 0; i < triggeredEvents; i++)
            {
                // Controllo se l'evento è relativo alla FIFO del client
                if (events[i].data.fd == fifoFd)
                {
                    if (events[i].events & EPOLLHUP)
                    {
                        customErrorPrinting("[ERROR] La FIFO è stata chiusa dal client\n");
                        quittingGame = 1;
                        break;
                    }
                    else if (events[i].events & EPOLLIN)
                    {
                        // Lettura dalla FIFO
                        int fifoRead = customFifoRead(fifoBuffer);
                        // Aggiunta dei meteoriti alla griglia di gioco
                        addMeteors(fifoBuffer);
                        // Ridisegno la griglia di gioco ad ogni aggiunta di meteoriti
                        redrawRowsTicker();
                    }
                }
                else if (events[i].data.fd == STDIN_FILENO && events[i].events & EPOLLIN) // Controllo se l'evento è relativo all'input da tastiera
                {
                    // Lettura da tastiera
                    char c[10];
                    int n = read(STDIN_FILENO, c, 9);
                    // Ridisegno la griglia di gioco ad ogni input ed avazo la riga di meteoriti
                    redrawRowsTicker();
                    quittingGame = checkTheInput(c, n);
                    if (quittingGame == 1)
                    {
                        gameOver = 1;
                        sleep(2);           // Attesa per la visualizzazione del game over
                        setTerminalMode(0); // Modalità normale del terminale
                        // Pulizia della griglia di gioco
                        printf("\033[H\033[J");
                        printf("[INFO] Uscita dal gioco in corso\n");
                        // Invio del messaggio di game over al client
                        sleep(2);
                        int bytesToClient = customFifoWrite(gameOverString);
                        printf("[INFO] Inviati %d byte al client: %s\n", bytesToClient, gameOverString);
                        sleep(2);
                        exit(EXIT_SUCCESS);
                    }
                }
            }
        }
    }
    return 0;
}