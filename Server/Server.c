/*
    Server di gioco:
        - Bastato su socket UDP
*/
#include "lib/wrappers/basicWrappers.h"
#include "lib/argChecker.h"
#include <netinet/in.h>

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

    // ciclo di attesa per i client
    while (1)
    {
    }

    return 0;
}
