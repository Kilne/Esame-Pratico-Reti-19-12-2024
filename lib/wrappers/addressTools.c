/*
    Collezione di helpers e warppers per la gestione degli indirizzi
    utilizza la libreria di sistema per la conversione degli indirizzi
    in network order e viceversa, in particolare la funzione inet_aton
    e inet_ntop
*/
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include "customErrorPrinting.h"
#include "../argChecker.h"
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
        customErrorPrinting("Errore LOCALHOST: conversione indirizzo IP non riuscita:\n");
        exit(EXIT_FAILURE);
    }
}

/*
    Imposta l'indirizzo in un struttura in_addr con un generico indirizzo IP in network order
    prevede un indirizzo in dotted decimal notation @ip
*/
extern void setGenericIPV4(char *ip, struct in_addr *addr)
{
    if (inet_aton(ip, addr) == 0)
    {
        customErrorPrinting("Errore IPV4: conversione indirizzo IP non riuscita\n");
        exit(EXIT_FAILURE);
    }
}
/*
    Ritorna l'indirizzo dotted di un generico indirizzo IP in network order
*/
extern char *getIPV4HostDecimal(struct in_addr *addr)
{
    static char ip[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN) == NULL)
    {
        customErrorPrinting("Errore GETIPV4: conversione ad indirizzo IP non riuscita\n");
        exit(EXIT_FAILURE);
    }
    return ip;
}
/*
    Ritorna una porta casuale tra 49152 e 65535
    @return: porta in network order
*/
extern uint16_t getRadnomPort()
{
    // Inizializzazione del generatore di numeri casuali
    srand(time(NULL));

    // Generazione di un numero casuale tra 49152 e 65535
    short port = rand() % (65535 - 49152 + 1) + 49152;

    // Porta in network order
    return htons(port);
}
/*
    Restituisce un indirizzo IP e porta in formato stringa
    valido solo per IPV4.
    @return: stringa con indirizzo e porta
*/
extern char *getAddressString(struct sockaddr_in *addr)
{
    // Buffer per l'indirizzo e la porta
    static char address[INET_ADDRSTRLEN + 6];
    // Conversione dell'indirizzo IP e della porta in stringa
    char *ip = getIPV4HostDecimal(&addr->sin_addr);
    sprintf(address, "%s:%d", ip, ntohs(addr->sin_port));

    return address;
}
/*
    Funzione per impostazione manuale di una porta
    @serverAddr: struttura dati per l'indirizzo del server
*/
extern void setPortManually(struct sockaddr_in *serverAddr)
{

    // Immissione della porta nella struttura dati in maniera dinamicamente allocata con getline
    char *port = NULL;
    printf("Inserire la porta del server: ");
    if (getline(&port, &(size_t){0}, stdin) == -1)
    {
        customErrorPrinting("GETLINE: non è riuscita a convertire la stringa \n");
        exit(EXIT_FAILURE);
    }
    // Rimozione trailing newline
    port[strlen(port) - 1] = '\0';
    checkPort(port);

    // Conversione della porta in network order
    serverAddr->sin_port = htons(atoi(port));

    // Deallocazione della memoria
    free(port);
}