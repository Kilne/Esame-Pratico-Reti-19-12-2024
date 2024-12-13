/*
    Collezione di wrappers per la creazione e gestione delle system call
    comuni per la creazione di socket, binding e impostazioni del socket
    ed altre funzioni di utilità.
*/
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <netinet/in.h>
#include "customErrorPrinting.h"
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
        customErrorPrinting("socket: Errore di creazione del socket\n");
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
extern void wrappedBind(int sockfd, struct sockaddr_in addr)
{
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        customErrorPrinting("bind: Errore di binding dell'indirizzo sul socket\n");
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
    timeout.tv_usec = 0;

    /*
        Aggiunta opzioni per il socket:
            - SO_REUSEADDR: riutilizzo dell'indirizzo
            - SO_RCVTIMEO: timeout per la ricezione dei messaggi
            - IP_RECVERR: ricezione degli errori di rete ICMP asincroni per il socket UDP
    */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        customErrorPrinting("[ERROR] setsockopt: SO_REUSEADDR\n");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        customErrorPrinting("[ERROR] setsockopt: SO_RCVTIMEO\n");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_IP, IP_RECVERR, &enableReuse, sizeof(int)) < 0)
    {
        customErrorPrinting("[ERROR] setsockopt: IP_RECVERR\n");
        exit(EXIT_FAILURE);
    }
}