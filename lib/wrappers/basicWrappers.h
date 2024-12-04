/*
    Collezzione degli header per funzioni wrapper per le system call di base
*/
#ifndef LIB_WARPPERS_BASICWRAPPERS_H
#define LIB_WARPPERS_BASICWRAPPERS_H
#include <netinet/in.h>
// Crea e restiuisce un valore intero per il file descriptor del socket.
extern int wrappedSocket(int domain, int type);

// Associa un indirizzo ad un socket
extern void wrappedBind(int sockfd, struct sockaddr_in addr);

// Imposta le opzioni del socket
extern void wrappedSocketOpt(int sockfd);

#endif // LIB_WARPPERS_BASICWRAPPERS_H