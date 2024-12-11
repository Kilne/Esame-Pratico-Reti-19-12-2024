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
#include "../lib/wrappers/pollUtils.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTS 50

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
    struct sockaddr_in clientAddr[MAX_CLIENTS];
    char *dummyIPdecimals = "255.255.255.255";
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        // Metto tutti gli indirizzi a un valore non valido per i miei scopi
        setGenericIPV4(dummyIPdecimals, &clientAddr[i].sin_addr);
        clientAddr[i].sin_port = htons(0);
    }

    // Inzializzazione del buffer che contiene le future posizioni per i meteoriti
    // Generate casualmente
    char *meteoritesBuffer = calloc(20, sizeof(char));

    /*
        TODO:
            - IL sistema di game start è stupido dovresti mettere in lista i vari client e quando sono pronti iniziare il gioco
            mandargli a tutti il nuovo set di meteoriti e poi aspettare che finiscano di giocare
            - Quando un client si disconnette rimuoverlo dalla lista, il client dovrebbe mandare un messaggio di disconnessione
    */
    // Inizializzazione dell'epoll
    startEpoll();
    addFileDescriptorToThePolling(serverSocket, EPOLLIN | EPOLLOUT);

    // ciclo di attesa per i client
    while (1)
    {
        // Ogni 2 si generano nuove posizioni per i meteoriti
        sleep(2);
        generateMeteorites(meteoritesBuffer, 20);
        printf("[INFO] Nuove posizioni per i meteoriti generate: %s\n", meteoritesBuffer);

        // Attesa degli eventi di I/O
        struct epoll_event fdEvents[10];
        int triggeredEvents = waitForEvents(fdEvents, 10);

        for (int i = 0; i < triggeredEvents; i++)
        {
            if (fdEvents->events & EPOLLIN)
            {
                // Controllo chi è il client che ha inviato il messaggio
                struct sockaddr_in currentClient;
                customRecvFrom(serverSocket, messageBuffer, &currentClient);  // Ricevo il messaggio
                char *currentAddrComplete = getAddressString(&currentClient); // Estraggo l'indirizzo completo in formato "IP:PORT"
                printf("[INFO] Messaggio ricevuto: %s da %s\n", messageBuffer, currentAddrComplete);

                // Controllo che messaggio ho ricevuto, nel caso di START accetto il client
                if (strcmp(messageBuffer, "START") == 0)
                {
                    // Vedo se lo devo aggiungere alla lista dei client
                    int clientAlreadyInList = 0;
                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        // Il confronto include le porte.
                        char *arrayAddr = getAddressString(&clientAddr[j]);
                        if (strcmp(arrayAddr, currentAddrComplete) == 0)
                        {
                            clientAlreadyInList = 1;
                            printf("[INFO] Client già in lista\n");
                            free(arrayAddr);
                            break;
                        }
                        free(arrayAddr);
                    }
                    if (clientAlreadyInList == 0)
                    {
                        printf("[INFO] Aggiungo il client alla lista\n");

                        // Trovo il primo slot libero per il client
                        for (int h = 0; h < MAX_CLIENTS; h++)
                        {

                            // Confronto con l'indirizzo presente nella struttura
                            if (clientAddr[h].sin_port == 0)
                            {
                                clientAddr[h] = currentClient;
                                printf("[INFO] Client aggiunto alla lista in posizione %d\n", h);
                                break;
                            }
                        }
                    }
                    // Scarico il buffer del messaggio del client
                    freeUDPMessage(messageBuffer);
                    messageBuffer = getStdUDPMessage();
                }
                free(currentAddrComplete);
            }
            else if (fdEvents->events & EPOLLOUT)
            {
                // Invio le posizioni, se un client in lista non è più
                // valido lo rimuovo.
                for (int j = 0; j < MAX_CLIENTS; j++)
                {
                    // Estraggo l'indirizzo IP del client dall'array di strutture
                    char *arrayAddr = getAddressString(&clientAddr[j]);

                    // Controllo se effettivamente c'è un client in lista da poter inviare il messaggio
                    if (strcmp(arrayAddr, "255.255.255.255:0") == 0)
                    {
                        free(arrayAddr);
                        continue;
                    }
                    else
                    {
                        printf("[INFO] Invio posizioni dei meteoriti a %s\n", arrayAddr);
                        // Invio delle posizioni dei meteoriti al client
                        if (customSendTo(serverSocket, meteoritesBuffer, &clientAddr[j]) == 1)
                        {
                            // Rendo il client della lista inutilizzabile per le future comunicazioni se non raggiungibile
                            printf("[INFO] Il client %s non è più raggiungibile, rimozione.\n", arrayAddr);
                            clientAddr[j].sin_family = AF_UNSPEC;
                            setGenericIPV4("255.255.255.255", &clientAddr[j].sin_addr);
                            clientAddr[j].sin_port = htons(0);
                        }
                        else
                        {
                            printf("[INFO] Posizioni inviate con successo a: %s\n", arrayAddr);
                        }
                    }
                    free(arrayAddr);
                }
            }
        }
        // Pulizia del buffer
        memset(messageBuffer, 0, 20);
    }

    return 0;
}
