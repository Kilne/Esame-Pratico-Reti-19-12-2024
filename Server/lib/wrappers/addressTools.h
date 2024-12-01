/*
    Collezione di helpers e wrappers per la gestione degli indirizzi
*/

#ifndef SERVER_LIB_WARPPERS_ADDRESSTOOLS_H
#define SERVER_LIB_WARPPERS_ADDRESSTOOLS_H

// Imposta un indirizzo LOCALHOST in network order
extern void setLocalHostIPV4(struct in_addr *addr);
// Imposta l'indirizzo in un struttura in_addr con un generico indirizzo IPV4 in network order
extern void setGenericIP4(char *ip, struct in_addr *addr);
// Ritorna l'indirizzo dotted di un generico indirizzo IP in network order
extern char * getIPV4HostDecimal(struct in_addr *addr);

#endif // SERVER_LIB_WARPPERS_ADDRESSTOOLS_H