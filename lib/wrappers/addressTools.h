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
extern char *getIPV4HostDecimal(struct in_addr *addr);
// Ritorna una porta casuale tra 49152 e 65535
extern uint16_t getRadnomPort();
// Ritorna una stringa con l'indirizzo e la porta in formato "IP:PORT"
extern char *getAddressString(struct sockaddr_in *addr);
// Allocazione e inizializzazione manuale di una porta per la struttura sockaddr_in
extern void setPortManually(struct sockaddr_in *serverAddr);

#endif // LIB_WARPPERS_ADDRESSTOOLS_H