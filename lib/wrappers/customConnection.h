/*
    File header per la gestione delle connessioni personalizzate su UDP con connect.
*/
#ifndef LIB_WRAPPERS_CUSTOMCONNECTION_H
#define LIB_WRAPPERS_CUSTOMCONNECTION_H

#include <netinet/in.h>
extern void customConnection_init(int socket, struct sockaddr_in *serverAddr);
extern void customDisconnect(int socket);

#endif // LIB_WRAPPERS_CUSTOMCONNECTION_H