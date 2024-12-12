/*
    Questo file contiene funzione per la risoluzione dei nomi attraverso il servizio di DNS.
    Attraverso la funzione getaddrinfo().

    Risorse esterne:
    - https://www.man7.org/linux/man-pages/man3/getaddrinfo.3.html
*/
#define _GNU_SOURCE // Per funzioni extra
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>

/*
    Funzione per settare che tipologia fra le comuni connessioni di rete si vuole ricercare.
    @param whatYouWant: cosa si vuole cercare
    @return: la struttura con i suggerimenti per la ricerca
*/
extern struct addrinfo *setHints(char *whatYouWant)
{
    // A seconda di cosa ho ricevuto come argomento
    // imposto i suggerimenti per la ricerca
    int family = AF_UNSPEC;
    int sockType = SOCK_STREAM;
    int flags = 0;
    /*
        Flag:
        - AI_ADDRCONFIG: SOLO indirizzi configurati sulla macchina
        - AI_CANONNAME: nome canonico
    */
    if (strcmp(whatYouWant, "LOCALHOST"))
    {
        family = AF_INET;
        sockType = SOCK_STREAM;
        flags = AI_ADDRCONFIG | AI_CANONNAME;
    }
    else if (strcmp(whatYouWant, "REMOTEHOST"))
    {
        family = AF_INET;
        sockType = SOCK_STREAM;
        flags = AI_ADDRCONFIG | AI_CANONNAME;
    }
    else if (strcmp(whatYouWant, "REMOTEUDP"))
    {
        family = AF_INET;
        sockType = SOCK_DGRAM;
        flags = AI_ADDRCONFIG | AI_CANONNAME;
    }
    else if (strcmp(whatYouWant, "LOCALUDP"))
    {
        family = AF_INET;
        sockType = SOCK_DGRAM;
        flags = AI_ADDRCONFIG | AI_CANONNAME;
    }
    else
    {
        family = AF_UNSPEC;
        sockType = SOCK_STREAM;
        flags = AI_ADDRCONFIG | AI_CANONNAME;
    }

    // Struttura per gli hint
    struct addrinfo *hints = calloc(1, sizeof(struct addrinfo));
    hints->ai_family = family;     // Famiglia di indirizzi
    hints->ai_socktype = sockType; // Tipo di socket
    hints->ai_flags = flags;       // Flags per la ricerca
    hints->ai_protocol = 0;        // Protocollo qualunque
    hints->ai_canonname = NULL;    // Nome canonico
    hints->ai_addr = NULL;         // Indirizzo
    hints->ai_next = NULL;         // Puntatore al prossimo elemento
    return hints;
}
/*
    Funzione per la risoluzione del nome del server.
    @param serverName: nome del server da risolvere o indirizzo IP, può essere NULL per certi tipi di servizio
    @param typeOfService: tipo di servizio che si vuole risolvere per generare hint
        - LOCALHOST: per il server locale, LOCALUDP: per il server locale UDP
        - REMOTEHOST: per il server remoto, REMOTEUDP: per il server remoto UDP
    @return: la struttura con i risultati della ricerca
*/
extern struct addrinfo **resolveMyName(char *serverName, char *typeOfService)
{
    // Struttura per i risultati e per i suggerimenti
    struct addrinfo **results;
    struct addrinfo *hints;
    // Risoluzione del nome

    if (serverName == NULL)
    {
        // LOCALHOST
        if (strcmp(typeOfService, "LOCALHOST") == 0)
        {
            hints = setHints("LOCALHOST");
            char *localHostDecimals; // TODO Continua da qui
        }
    }