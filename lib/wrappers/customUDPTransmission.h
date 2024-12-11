/*
    File header per la gestione delle comunicazioni personalizzate su UDP con connect.
*/

#ifndef LIB_WRAPPERS_CUSTOMUDPTRANSMISSION_H
#define LIB_WRAPPERS_CUSTOMUDPTRANSMISSION_H

#include <netinet/in.h>
// Funzione per l'invio di un messaggio al server con controllo ICMP
extern int customSend(int socket, struct sockaddr_in *server, char *buffer);
// Funzione per la ricezione di un messaggio dal server
extern void customRecv(int socket, char *buffer);

#endif // LIB_WRAPPERS_CUSTOMUDPTRANSMISSION_H