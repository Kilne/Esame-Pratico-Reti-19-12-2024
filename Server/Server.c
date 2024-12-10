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
#include "../lib/gamelogics/meteorites.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    struct sockaddr_in clientAddr[50];
    int lastClient = 0;

    // Inzializzazione del buffer che contiene le future posizioni per i meteoriti
    // Generate casualmente
    char *meteoritesBuffer = calloc(20, sizeof(char));

    /*
        TODO:
            - IL sistema di game start è stupido dovresti mettere in lista i vari client e quando sono pronti iniziare il gioco
            mandargli a tutti il nuovo set di meteoriti e poi aspettare che finiscano di giocare
            - Quando un client si disconnette rimuoverlo dalla lista, il client dovrebbe mandare un messaggio di disconnessione
    */

    // ciclo di attesa per i client
    while (1)
    {
        // Ogni 2 si generano nuove posizioni per i meteoriti
        sleep(2);
        generateMeteorites(meteoritesBuffer, 20);
        printf("[INFO] Nuove posizioni per i meteoriti generate: %s\n", meteoritesBuffer);
        // Ricezione del messaggio dal client
        customRecvFrom(serverSocket, messageBuffer, &clientAddr[lastClient]);
        if (strcmp(messageBuffer, "START") == 0)
        {
            // Invio delle posizioni dei meteoriti ai client
            printf("[INFO] Nuovo client nella coda.\n");
            lastClient++;
            if (lastClient > 50)
            {
                lastClient = 50;
            }
            freeUDPMessage(messageBuffer);
            messageBuffer = getStdUDPMessage();
        }

        for (int i = 0; i < 50; i++)
        {
            if (&clientAddr[i] == NULL)
            {
                continue;
            }
            // Invio delle posizioni dei meteoriti al client
            customSendTo(serverSocket, meteoritesBuffer, &clientAddr[i]);
        }

        memset(messageBuffer, 0, 20);
    }

    return 0;
}
