/*
    Client di gioco per la comunicazione con il server
*/

#include "../lib/wrappers/customRecvFrom.h"
#include "../lib/wrappers/customSendTo.h"
#include "../lib/wrappers/basicWrappers.h"
#include "../lib/argChecker.h"
#include <netinet/in.h>
#include <stdio.h>

#define MAX_MESSAGE_SIZE 1024

int main(int argc, char const *argv[])
{
    // Controllo degli argomenti
    checkArgs(argc);

    // Creazione del socket e opzioni per il riuso del socket e timeout
    int clientSocket = wrappedSocket(AF_INET, SOCK_DGRAM);
    wrappedSocketOpt(clientSocket);

    // TODO : tutto il resto, controlla la questione degli errori con perror e errno
    // Server a cui mandare e ricevere i messaggi
    struct sockaddr_in serverAddr;
    setTheServerAddress(argv, &serverAddr);

    // Impostazione del buffer per la ricezione dei messaggi
    char messageBuffer[MAX_MESSAGE_SIZE];

    // Impostazione della struttura per la ricezione dell'indirizzo del server da messaggi
    // ricevuti per controlli.
    struct sockaddr_in serverAddrReceived;

    // Ciclo di gioco
    while (1)
    {
        /* code */
    }

    return 0;
}
