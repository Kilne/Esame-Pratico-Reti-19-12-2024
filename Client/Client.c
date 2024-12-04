/*
    Client di gioco per la comunicazione con il server
*/

#include "lib/argChecker.h"
#include "lib/wrappers/addressTools.h"
#include "lib/wrappers/pollUtils.h"
#include "lib/wrappers/basicWrappers.h"
#include <netinet/in.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    // Controllo degli argomenti
    checkArgs(argc);

    // Creazione del socket e opzioni per il riuso del socket e timeout
    int clientSocket = wrappedSocket(AF_INET, SOCK_DGRAM);
    wrappedSocketOpt(clientSocket);

    // TODO : tutto il resto, controlla la questione degli errori con perror e errno

    return 0;
}
