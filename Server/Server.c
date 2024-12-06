/*
    Server di gioco:
        - Bastato su socket UDP
*/
#include "../lib/wrappers/basicWrappers.h"
#include "../lib/argChecker.h"
#include "../lib/wrappers/customRecvFrom.h"
#include "../lib/wrappers/customSendTo.h"
#include "../lib/wrappers/addressTools.h"
#include "../lib/wrappers/bufHandlers.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{

    // Controllo degli argomenti
    checkArgs(argc);

    // Creazione del socket e opzioni per il riuso del socket e timeout
    int serverSocket = wrappedSocket(AF_INET, SOCK_DGRAM);
    wrappedSocketOpt(serverSocket);

    /*
    Inizializzazione della struttura per l'indirizzo del server,
    porta generata casualmente
    */
    struct sockaddr_in serverAddr;
    setTheServerAddress(argv, &serverAddr);
    serverAddr.sin_port = getRadnomPort();
    printf("[INFO] Server impostato all'indirizzo: %s\n", getAddressString(&serverAddr));

    // Binding del socket
    wrappedBind(serverSocket, serverAddr);
    printf("[INFO] Server in ascolto sulla porta: %d\n", ntohs(serverAddr.sin_port));

    // Inizializzazione del buffer per la ricezione dei messaggi
    char *messageBuffer = getStdUDPMessage();

    // Inizializzazione della struttura per la ricezione dell'indirizzo del client da messaggi
    struct sockaddr_in clientAddr;

    // ciclo di attesa per i client
    while (1)
    {
        // Ricezione del messaggio dal client
        customRecvFrom(serverSocket, messageBuffer, &clientAddr);
        if (messageBuffer[0] == '\0')
        {
            continue;
        }
        else
        {
            // Stampa del messaggio ricevuto
            printf("[INFO] Messaggio ricevuto dal client: %s\n", messageBuffer);
            printf("[INFO] Client connesso all'indirizzo: %s\n", getAddressString(&clientAddr));
            memset(messageBuffer, 0, strlen(messageBuffer));
        }
    }

    return 0;
}
