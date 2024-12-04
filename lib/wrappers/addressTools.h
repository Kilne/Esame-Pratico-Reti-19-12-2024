/*
    Collezione di helpers e wrappers per la gestione degli indirizzi
*/

#ifndef LIB_WARPPERS_ADDRESSTOOLS_H
#define LIB_WARPPERS_ADDRESSTOOLS_H
#include <netinet/in.h>
// Imposta un indirizzo LOCALHOST in network order
extern void setLocalHostIPV4(struct in_addr *addr);
// Imposta l'indirizzo in un struttura in_addr con un generico indirizzo IPV4 in network order
extern void setGenericIPV4(char *ip, struct in_addr *addr);
// Ritorna l'indirizzo dotted di un generico indirizzo IP in network order
extern char * getIPV4HostDecimal(struct in_addr *addr);

#endif // LIB_WARPPERS_ADDRESSTOOLS_H