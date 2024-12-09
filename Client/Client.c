/*
    Client di gioco per la comunicazione con il server
*/

#include "../lib/wrappers/basicWrappers.h"
#include "../lib/argChecker.h"
#include "../lib/wrappers/addressTools.h"
#include "../lib/wrappers/bufHandlers.h"
#include "../lib/wrappers/customUDPTransmission.h"
#include "../lib/wrappers/customErrorPrinting.h"
#include "../lib/wrappers/customConnection.h"
#include "../lib/wrappers/pollUtils.h"
#include "../lib/wrappers/customFifoTools.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>

#define PID_PATH "/tmp/gamePid" // Questo è il file in cui scrivo il PID del motore di gioco

int main(int argc, char const *argv[])
{
    // Eliminazione della FIFO se esiste in uscita
    atexit(deleteFifo);

    // Controllo degli argomenti
    checkArgs(argc);

    // Creazione del socket e opzioni per il riuso del socket
    int clientSocket = wrappedSocket(AF_INET, SOCK_DGRAM);
    wrappedSocketOpt(clientSocket);

    // Server a cui mandare e ricevere i messaggi
    struct sockaddr_in serverAddr;
    setTheServerAddress(argv, &serverAddr);
    setPortManually(&serverAddr);

    // Inizialiazzazione del polling
    startEpoll();

    // Aggiunta del socket al polling in modalità di lettura inzialmente
    addFileDescriptorToThePolling(clientSocket, EPOLLIN);

    // Impostazione della struttura per la ricezione dell'indirizzo del server da messaggi
    // ricevuti per controlli.
    struct sockaddr_in serverAddrReceived;

    /*
        Inizializzazione del buffer per la ricezione della posizione delle meteore
        è un array a 2 dimensioni di 20 righe. Teoricamente il client non dovrebbe
        sapere a priori la dimensione del buffer in arrivo ( standard UDP a 512 byte)
        e dovrebbe essere in grado di riallocare la memoria per il buffer in arrivo.
    */
    int meteorStored = 0; // Contatore per i buffer delle meteore
    // Per scelta di design del gioco massimo 20 buffer di meteore
    char **messageBuffer = calloc(20, sizeof(char *));

    // Special connect
    customConnection_init(clientSocket, &serverAddr);

    // Inzia il gioco
    customSend(clientSocket, "START");

    /*
        TODO:
            - Implementare l'apertura del terminale                                                                 [DONE]
            - Avvviare il gioco                                                                                     [DONE]
            - Ristrutturare il buffer delle meteore in maniera tale da avere max 20 buffer da mandare al gioco      [DONE]
                - Oltre i venti buffer i messaggi vanno scartati                                                    [DONE]
                - Ogni volta che mandi una FIFO al gioco libera il buffer per un messaggio                          [DONE?]
            - Utilizzo della FIFO per la comunicazione con il gioco                                                 [DONE]
            - Implementare la terminazione del gioco, con Epoll emettere un evento sulla FIFO e far chiudere il Client(previa disconnect)
                attraverso un ultimo messaggio al client per togliere la propria entry dalla lista di quelli in ascolto.
            - Indagare sulla possibile morte del server con errore ICMP e propagare l'errore al gioco ed il client,
                probabilemnte un timeout -> retry -> ICMP x 3 -> terminazione
            - Chiusura/Delete della FIFO al termine del gioco/client con atexit                                     [DONE]
            - Risoluzione DNS del server con getaddrinfo
            - Dato che dovrei terminare il client in maniera "Graziosa" interroppendo il cilo while, fare a fine
                ciclo un repulisti di memoria:
                - Libero i buffer delle meteore                         [DONE]
                - Chiudo il socket                                      [DONE]
                - Chiudo la FIFO                                        [DONE]
                - Chiudo l'istanza di epoll                             [DONE]
                - Libero la memoria allocata per la struttura del server[DONE]
    */

    // Apertura del terminal con spawn di un processo figlio dedito al gioco
    int systemResult = 0;
#ifdef __linux__
    systemResult = system("gnome-terminal -- ./Game");
    if (systemResult == -1)
    {
        customErrorPrinting("[ERROR] Apertura del terminale fallita\n");
        exit(EXIT_FAILURE);
    }
#elif __APPLE__
    systemResult = system("open -a Terminal ./Game");
    if (systemResult == -1)
    {
        customErrorPrinting("[ERROR] Apertura del terminale fallita\n");
        exit(EXIT_FAILURE);
    }
#endif
    // Lettura del PID del gioco
    pid_t gamePid = 0;
    FILE *pidFile = fopen(PID_PATH, "r");
    if (pidFile == NULL)
    {
        customErrorPrinting("[ERROR] fopen(): Errore nell'apertura del file PID\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fscanf(pidFile, "%d", &gamePid);
        fclose(pidFile);
        printf("[INFO] PID del gioco: %d\n", gamePid);
    }

    // Inizializzazione della pipeline per la comunicazione con il gioco
    createFifo();
    setFifoFd();
    addFileDescriptorToThePolling(getFifoFd(), EPOLLIN | EPOLLOUT); // Aggiunta della FIFO al polling

    // Segnale per la terminazione del gioco
    int gameTerminated = 0;
    // Ciclo di gioco
    while (gameTerminated == 0)
    {
        // Attesa degli eventi di I/O, valore arbitrario di 156
        struct epoll_event fdEvents[150];
        int triggeredEvents = waitForEvents(fdEvents, 150);

        // Logica evento per evento
        for (int i = 0; i < triggeredEvents; i++)
        {
            // Controllo se il socket è pronto per la lettura
            if (fdEvents[i].data.fd == clientSocket && EPOLLIN)
            {
                // Ricezione del messaggio
                // Per scelta di design del gioco il client scarta i messaggi se ha già 20 buffer
                if (meteorStored < 20)
                {
                    // Se posso ricevere il buffer, lo ricevo allocando un buffer standard UDP
                    // alla posizione meteorStored
                    messageBuffer[meteorStored] = getStdUDPMessage();

                    // Il contatore delle meteore è anche indice di quale è il buffer da riempire
                    customRecv(clientSocket, messageBuffer[meteorStored]);

                    // Incremento il contatore delle meteore
                    meteorStored++;
                }
            }
            // Controllo se la FIFO è pronta per la scrittura
            else if (fdEvents[i].data.fd == getFifoFd() && EPOLLOUT)
            {
                // Invio del buffer alla FIFO
                int bytesSentToFifo = 0;
                bytesSentToFifo = (getFifoFd(), messageBuffer[meteorStored], 20);
                if (bytesSentToFifo == -1)
                {
                    customErrorPrinting("[ERROR] write(): Errore nell'invio del buffer alla FIFO\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    /*
                        TODO:
                            - Dopo aver inviato il buffer tramite FIFO liberare il buffer
                                ad operazione compiuta. e decrementare il contatore delle meteore. [DONE?]
                    */
                    // Libero il buffer se è possibile
                    if (meteorStored > 0)
                    {
                        freeUDPMessage(messageBuffer[meteorStored]);
                        meteorStored--;
                    }
                }
            }
            // Controllo FIFO in lettura
            else if (fdEvents[i].data.fd == getFifoFd() && EPOLLIN)
            {
                // Lettura dalla FIFO
                char *buffer = "GAME OVER";
                char *bufferReceived = calloc(10, sizeof(char));
                int bytesReceived = read(getFifoFd(), bufferReceived, 10);
                if (bytesReceived == -1)
                {
                    customErrorPrinting("[ERROR] read(): Errore nella lettura dalla FIFO\n");
                    exit(EXIT_FAILURE);
                }
                if (strcmp(buffer, bufferReceived) == 0)
                {
                    // Game over procedi alla terminazione del gioco
                    printf("[INFO] Terminazione del gico in corso\n");
                    if (kill(gamePid, SIGKILL) == -1)
                    {
                        customErrorPrinting("[ERROR] kill(): Errore nella terminazione del gioco\n");
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        gameTerminated = 1;
                    }
                }
            }
        }
    }

    // Disconnessione dal server
    customDisconnect(clientSocket);

    // Chiusura dell'istanza di epoll
    removeFileDescriptorFromThePolling(clientSocket);
    removeFileDescriptorFromThePolling(getFifoFd());
    closeEpoll();

    // Chiusura della FIFO
    deleteFifo();

    // Chiusura del socket
    if (close(clientSocket) == -1)
    {
        customErrorPrinting("[ERROR] close(): Errore nella chiusura del socket\n");
        exit(EXIT_FAILURE);
    }

    // Liberazione della memoria allocata per la struttura del server
    free(messageBuffer);
    free(&serverAddr);

    return 0;
}
