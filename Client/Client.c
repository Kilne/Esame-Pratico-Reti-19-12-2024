/*
    Client di gioco per la comunicazione con il server
*/

#include "../lib/wrappers/customRecvFrom.h"
#include "../lib/wrappers/customSendTo.h"
#include "../lib/wrappers/basicWrappers.h"
#include "../lib/argChecker.h"
#include "../lib/wrappers/addressTools.h"
#include "../lib/wrappers/bufHandlers.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char const *argv[])
{
    // Controllo degli argomenti
    checkArgs(argc);

    // Creazione del socket e opzioni per il riuso del socket e timeout
    int clientSocket = wrappedSocket(AF_INET, SOCK_DGRAM);
    wrappedSocketOpt(clientSocket);

    // Server a cui mandare e ricevere i messaggi
    struct sockaddr_in serverAddr;
    setTheServerAddress(argv, &serverAddr);
    setPortManually(&serverAddr);

    // Impostazione della struttura per la ricezione dell'indirizzo del server da messaggi
    // ricevuti per controlli.
    struct sockaddr_in serverAddrReceived;
    // Inizializzazione del buffer per la ricezione dei messaggi
    char *messageBuffer = getStdUDPMessage();

    // Special connect
    int val = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    fprintf(stdout, "[INFO] Connect value: %d\n", val);
    int sent = send(clientSocket, "CONNECT", 7, 0);
    fprintf(stdout, "[INFO] Sent value: %d\n", sent);
    struct sockaddr_in disconnected;
    disconnected.sin_family = AF_UNSPEC;
    int val2 = connect(clientSocket, (struct sockaddr *)&disconnected, sizeof(disconnected));
    fprintf(stdout, "[INFO] Connect value: %d\n", val2);

    // Ciclo di gioco
    while (1)
    {
    }

    return 0;
}
