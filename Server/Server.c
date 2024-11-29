/*
    Server di gioco:
        - Bastato su socket UDP
        - Concorrente
*/
#include "lib/warppers/basicWrappers.h"
#include <arpa/inet.h>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    // Creazione del socket
    int serverSocket = wrappedSocket(AF_INET,SOCK_DGRAM);



    return 0;
}
