#include <sys/socket.h>
/*
    Semplice wrapper per creazione socket:
    @domain: famiglia/protocollo derivanti dalle costanti contenute nel file sys/socket.h
    @type: tipo di connessione derivante dalle costatni contenute nel file sys/socket.h
    @return: file descriptor del socket
*/
extern int wrappedSocket(int domain, int type) {

    // Protocollo default a zero per praticità
    int sockfd = socket(domain, type, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}
/*
*/
extern void wrappedBind(int sockfd, struct sockaddr_in *addr, socklen_t addrlen) {

    if (bind(sockfd,(struct sockaddr *) addr, addrlen) < 0) {
        perror("bind");
        exit(1);
    }
}