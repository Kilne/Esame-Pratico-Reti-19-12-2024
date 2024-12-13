/*
    Motore di gioco
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lib/wrappers/pollUtils.h"
#include "lib/gamelogics/drawingField.h"
#include "lib/wrappers/customQueIPC.h"
#include "lib/wrappers/customErrorPrinting.h"

int main(int argc, char const *argv[])
{
    // Controllo degli argomenti
    if (argc != 3)
    {
        customErrorPrinting("[ERROR] Numero di argomenti non valido\n");
        sleep(3);
        sendMessageToQueue(openQueue(argv[2]).fileDescriptor, "GAME OVER"); // Invio del messaggio di chiusura al client
    }

    // Inizializzazione della griglia di gioco
    init();

    // Acquisizione del file descriptor per le code
    // dagli argomenti passati si estraggono le parti randomiche per differenziare le code
    mq_open_data data = openQueue(argv[1]);
    mq_open_data data_2 = openQueue(argv[2]);
    int queFd = data.fileDescriptor;
    int queFd_2 = data_2.fileDescriptor;

    // Creo due buffer per le code
    char *fromClientBuffer = calloc(20, sizeof(char));

    // Inzializzazione Epoll, monitoraggio delle FIFO e dell'input da tastiera
    startEpoll();
    addFileDescriptorToThePolling(STDIN_FILENO, EPOLLIN);
    addFileDescriptorToThePolling(queFd, EPOLLIN);
    printf("[INFO] Epoll inizializzato\n");

    // Inizializzazione del monitoraggio dell'input da tastiera
    setTerminalMode(1); // Modalità raw

    // Controllo game over
    int gameOver = 0;                   // 0: no game over, 1: game over
    int quittingGame = 0;               // 0: non si sta uscendo dal gioco, 1: si sta uscendo dal gioco
    char *gameOverString = "GAME OVER"; // Stringa per il client

    // Ciclo di gioco
    while (gameOver == 0)
    {
        // Ad ogni ciclo ridisegno la griglia di gioco
        printGrid();

        // Eventi per FIFO e tastiera
        struct epoll_event events[10];

        // Attesa degli eventi di I/O
        int triggeredEvents = waitForEvents(events, 10);

        // Gestione degli eventi
        if (triggeredEvents > 0)
        {
            // Controllo evento
            for (int i = 0; i < triggeredEvents; i++)
            {
                // Controllo se l'evento è relativo alla coda del client
                if (events[i].data.fd == queFd)
                {
                    if (events[i].events & EPOLLHUP)
                    {
                        setTerminalMode(0);     // Rimetto il terminale in modalità normale
                        printf("\033[H\033[J"); // Pulizia del terminale
                        customErrorPrinting("[ERROR] La coda dal client al gioco è stata chiusa\n");
                        gameOver = 1;
                        quittingGame = 1;
                        break; // Esco dal ciclo for e torno al while
                    }
                    else if (events[i].events & EPOLLIN)
                    {
                        // Lettura dalla coda messaggi
                        fromClientBuffer = receiveMessageFromQueue(queFd);
                        if (fromClientBuffer == NULL || strcmp(fromClientBuffer, "DEAD") == 0)
                        {
                            setTerminalMode(0);     // Rimetto il terminale in modalità normale
                            printf("\033[H\033[J"); // Pulizia del terminale
                            printf("[ERROR] Server assente o problema con la coda messaggi.\n");
                            gameOver = 1;
                            quittingGame = 1;
                            break; // Esco dal ciclo for e torno al while
                        }
                        else if (strcmp(fromClientBuffer, "EMPTY") == 0)
                        {
                            // Coda vuota non faccio nulla e faccio andare avanti il gioco
                            continue;
                        }
                        else
                        {
                            // Aggiunta dei meteoriti alla griglia di gioco
                            addMeteors(fromClientBuffer);
                            // Ridisegno la griglia di gioco ad ogni aggiunta di meteoriti
                            gameOver = redrawRowsTicker();
                            if (gameOver == 1)
                            {
                                // Il gioco è finito
                                quittingGame = 1;
                                break; // Esco dal ciclo for e torna al while
                            }
                        }
                    }
                }
                else if (events[i].data.fd == STDIN_FILENO && events[i].events & EPOLLIN) // Controllo se l'evento è relativo all'input da tastiera
                {
                    // Lettura da tastiera
                    char c[10];
                    int n = read(STDIN_FILENO, c, 9);
                    quittingGame = checkTheInput(c, n);
                    if (quittingGame == 1)
                    {
                        gameOver = 1;
                        sleep(2);           // Attesa per la visualizzazione del game over
                        setTerminalMode(0); // Modalità normale del terminale
                        // Pulizia della griglia di gioco
                        printf("\033[H\033[J");
                        printf("[INFO] Uscita dal gioco in corso\n");
                        sleep(2);
                        break; // Esco dal ciclo for e torno al while
                    }
                }
            }
        }
    }

    // Chiusura del gioco
    if (quittingGame == 1)
    {
        // Chiusura dell'istanza di epoll
        removeFileDescriptorFromThePolling(STDIN_FILENO);
        removeFileDescriptorFromThePolling(queFd);
        closeEpoll();

        // Free dei buffer
        free(fromClientBuffer);

        // Invio il messaggio che sto chiudento il gioco al client così che possa terminare
        if (sendMessageToQueue(queFd_2, gameOverString) == 1)
        {
            // Coda piena( impossibile in questo caso ma per sicurezza)
            // Riprovo per cinque volte in brute force ignoro gli errori -1
            for (int i = 0; i < 5; i++)
            {
                if (sendMessageToQueue(queFd_2, gameOverString) == 0)
                {
                    break;
                }
            }
        }

        // Chiusura delle code ma non la distruzione se ne occupa il client
        closeTheQueue(queFd);
        closeTheQueue(queFd_2);
        // Chiusura del gioco
        exit(EXIT_SUCCESS);
    }

    return 0;
}