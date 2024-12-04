/*
    Server di gioco:
        - Bastato su socket UDP
*/
#include "../lib/wrappers/basicWrappers.h"
#include "../lib/argChecker.h"
#include "../lib/wrappers/pollUtils.h"
#include <netinet/in.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{

    // Controllo degli argomenti
    checkArgs(argc);

    // Creazione del socket e opzioni per il riuso del socket e timeout
    int serverSocket = wrappedSocket(AF_INET, SOCK_DGRAM);
    wrappedSocketOpt(serverSocket);

    /*
    Inizializzazione della struttura per l'indirizzo del server
    Nessuna porta è speicificata, quindi il sistema sceglierà una porta libera
    data dalla specifica protocollo UDP.
    */
    struct sockaddr_in serverAddr;
    setTheServerAddress(argv, &serverAddr);

    // Binding del socket
    wrappedBind(serverSocket, serverAddr);

    // Inizializzazione epoll
    startEpoll();

    // Aggiunta del file descriptor del socket alla lista di quelli da monitorare
    addFileDescriptorToThePolling(serverSocket);

    // ciclo di attesa per i client
    while (1)
    {
        // TODO: Wrapping poll/ppoll per la gestione dei client, del STDERR/STDIO lato server e invio e datagrammi
        // TODO: AGGIORNARE IL FULE MAKE
        // Inizio attesa degli eventi
        int socketEvents = waitForEvents();
    }

    return 0;
}
