#include <sys/socket.h>
#include <stdlib.h>
#include <sys/time.h>
/*
    Semplice wrapper per creazione socket:
    @domain: famiglia/protocollo derivanti dalle costanti contenute nel file sys/socket.h
    @type: tipo di connessione derivante dalle costatni contenute nel file sys/socket.h
    @return: file descriptor del socket
*/
extern int wrappedSocket(int domain, int type)
{
    // Protocollo default a zero per praticità
    int sockfd = socket(domain, type, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}
/*
    Semplice wrapper per la funzione bind:
    @sockfd: file descriptor del socket
    @addr: struttura contenente l'indirizzo del socket
    @addrlen: dimensione dell'indirizzo
*/
extern void wrappedBind(int sockfd, struct sockaddr_in *addr)
{
    // Casting e sizing dell'indirizzo internamente per praticità
    struct sockaddr *saddr = (struct sockaddr *)addr;
    if (bind(sockfd, saddr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}
/*
    Wrapper per annidare e nascondere due comuni impostazioni del socket: riutilizzo e timeout
    viene impostato solo il timeout in secondi, default 10 secondi.
    @sockfd: file descriptor del socket
*/
extern void wrappedSocketOpt(int sockfd)
{
    int enableReuse = 1;
    struct timeval timeout;
    timeout.tv_sec = 10;

    // Setting options for reuse and timeout
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        perror("setsockopt: SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt: SO_RCVTIMEO");
        exit(EXIT_FAILURE);
    }
}

/*
 */
extern wappedRecieve()
{
}