/*
    Collezione di helpers e warppers per la gestione degli indirizzi
    utilizza la libreria di sistema per la conversione degli indirizzi
    in network order e viceversa, in particolare la funzione inet_aton
    e inet_ntop
*/
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
// Indirizzo di loopback
#define LOCALHOST "127.0.0.1"

/*
    Imposta un indirizzo LOCALHOST in network order
    sulla struttura dati passata come parametro del tipo @struct sockaddr_in
*/
extern void setLocalHostIPV4(struct in_addr *addr)
{
    if (inet_aton(LOCALHOST, addr) == 0)
    {
        perror("Errore LOCALHOST: conversione indirizzo IP non riuscita:\n");
        strerror(errno);
        exit(EXIT_FAILURE);
    }
}

/*
    Imposta l'indirizzo in un struttura in_addr con un generico indirizzo IP in network order
    prevede un indirizzo in dotted decimal notation @ip
    vengono fatti controlli sull'indirizzo passato
*/
extern void setGenericIPV4(char *ip, struct in_addr *addr)
{
    if (inet_aton(ip, addr) == 0)
    {
        perror("Errore IPV4: conversione indirizzo IP non riuscita\n");
        strerror(errno);
        exit(EXIT_FAILURE);
    }
}
/*
    Ritorna l'indirizzo dotted di un generico indirizzo IP in network order
*/
extern char * getIPV4HostDecimal(struct in_addr *addr)
{
    static char ip[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN) == NULL)
    {
        perror("Errore GETIPV4: conversione ad indirizzo IP non riuscita\n");
        strerror(errno);
        exit(EXIT_FAILURE);
    }
    return ip;
}