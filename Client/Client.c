/*
    Client di gioco per la comunicazione con il server
*/

#include "../lib/wrappers/basicWrappers.h"
#include "../lib/argChecker.h"
#include "../lib/wrappers/addressTools.h"
#include "../lib/wrappers/bufHandlers.h"
#include "../lib/wrappers/customUDPTransmission.h"
#include "../lib/wrappers/customConnection.h"
#include "../lib/wrappers/pollUtils.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>

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
    // Inizializzazione del buffer per la ricezione dei messaggi
    char *messageBuffer = getStdUDPMessage();

    // Special connect
    customConnection_init(clientSocket, &serverAddr);

    // Inzia il gioco
    customSend(clientSocket, "START");

    // TODO : FARE TUTTA LA PRASSI DI GIOCO: FIFO, TERMINALE, ECC...

    // Ciclo di gioco
    while (1)
    {
        // Attesa degli eventi di I/O, valore arbitrario di 156
        struct epoll_event fdEvents[150];
        int triggeredEvents = waitForEvents(fdEvents, 150);

        // Controllo se il socket è pronto per la lettura
        for (int i = 0; i < triggeredEvents; i++)
        {
            if (fdEvents[i].events & EPOLLIN)
            {
                // Ricezione del messaggio
                customRecv(clientSocket, messageBuffer);
                freeUDPMessage(messageBuffer);      // Deallocazione del buffer
                messageBuffer = getStdUDPMessage(); // Riallocazione del buffer
            }
        }
    }

    return 0;
}
