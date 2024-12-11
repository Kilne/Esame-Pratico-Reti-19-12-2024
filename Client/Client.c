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
#include <unistd.h>

int main(int argc, char const *argv[])
{

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
    int meteorStored = -1; // Contatore per i buffer delle meteore parte da meno 1 per l'array 0 based
    // Per scelta di design del gioco massimo 20 buffer di meteore
    char **messageBuffer = calloc(20, sizeof(char *));

    // Buffer per la ricezione dei messaggi dalla FIFO
    char *bufferReceived = calloc(20, sizeof(char));

    // Special connect
    customConnection_init(clientSocket, &serverAddr);

    /*
        TODO:
            - Implementare l'apertura del terminale                                                                 [DONE]
            - Avvviare il gioco                                                                                     [DONE]
            - Ristrutturare il buffer delle meteore in maniera tale da avere max 20 buffer da mandare al gioco      [DONE]
                - Oltre i venti buffer i messaggi vanno scartati                                                    [DONE]
                - Ogni volta che mandi una FIFO al gioco libera il buffer per un messaggio                          [DONE?]
            - Utilizzo della FIFO per la comunicazione con il gioco                                                 [DONE]
            - Implementare la terminazione del gioco, con Epoll emettere un evento sulla FIFO e far chiudere il Client(previa disconnect)
                attraverso un ultimo messaggio al client per togliere la propria entry dalla lista di quelli in ascolto. [FAILED]
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
    // system("gnome-terminal -- bash -c 'cd Builds; ./Game; exec bash'");
    systemResult = system("gnome-terminal -- ./Game");
    if (systemResult == -1)
    {
        customErrorPrinting("[ERROR] Apertura del terminale fallita\n");
        exit(EXIT_FAILURE);
    }

    // Inizializzazione della pipeline per la comunicazione con il gioco
    createFifo();
    setFifoFd();
    printf("[INFO] FIFO aperta dal client su file descriptor: %d\n", getFifoFd());
    addFileDescriptorToThePolling(getFifoFd(), EPOLLIN | EPOLLOUT);

    // Segnale per la terminazione del gioco
    int gameTerminated = 0;

    // Inzia il gioco
    int sendRes = customSend(clientSocket, &serverAddr, "START");
    if (sendRes == 1)
    {
        customErrorPrinting("[ERROR] Il server non è raggiungibile\n");
        exit(EXIT_FAILURE);
    }
    // Stringa di controllo chiusura del gioco
    char *gameEndString = "GAME OVER";

    // Ciclo di gioco
    while (gameTerminated == 0)
    {
        // Attesa degli eventi di I/O
        struct epoll_event fdEvents[10];
        int triggeredEvents = waitForEvents(fdEvents, 10);

        // Logica evento per evento
        for (int i = 0; i < triggeredEvents; i++)
        {
            // Controllo se il socket è pronto per la lettura
            if (fdEvents[i].data.fd == clientSocket)
            {
                // Ricezione del messaggio
                // Per scelta di design del gioco il client scarta i messaggi se ha già 20 buffer
                if (meteorStored != 19)
                {
                    // Incremento il contatore delle meteore PRIMA di ricevere il buffer perchè è 0 based
                    meteorStored++;
                    // Se posso ricevere il buffer, lo ricevo allocando un buffer standard UDP
                    // alla posizione meteorStored
                    messageBuffer[meteorStored] = getStdUDPMessage();

                    // Il contatore delle meteore è anche indice di quale è il buffer da riempire
                    customRecv(clientSocket, messageBuffer[meteorStored]);
                    printf("[INFO] Ricevuto buffer dal server: %s\n", messageBuffer[meteorStored]);
                }
            }
            // Controllo se la FIFO è pronta
            else if (fdEvents[i].data.fd == getFifoFd())
            {
                if (fdEvents[i].events & EPOLLOUT)
                {
                    // Invio del buffer alla FIFO
                    int sentBytesToFifo = 0;
                    if (meteorStored != -1)
                    {
                        // RAGIONEVOLMENTE non dovrei passare array nulli alla FIFO con il contatore
                        sentBytesToFifo = customFifoWrite(messageBuffer[meteorStored]);
                        printf("[INFO] Inviati %d byte alla FIFO: %s\n", sentBytesToFifo, messageBuffer[meteorStored]);
                        // Libero il buffer per il prossimo messaggio
                        freeUDPMessage(messageBuffer[meteorStored]);
                        // Decremento il contatore delle meteore
                        meteorStored--;
                    }
                }
                else if (fdEvents[i].events & EPOLLIN)
                {
                    // Lettura dalla FIFO
                    int bytesReceived = customFifoRead(bufferReceived);
                    printf("[INFO] Letti %d byte dalla FIFO: %s\n", bytesReceived, bufferReceived);
                    if (strcmp(gameEndString, bufferReceived) == 0)
                    {
                        // Game over procedi alla terminazione del gioco
                        printf("[INFO] Terminazione del gico in corso\n");
                        gameTerminated = 1;
                    }
                }
            }
        }
    }

    if (gameTerminated == 1)
    {
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
        free(bufferReceived);
    }

    return 0;
}
