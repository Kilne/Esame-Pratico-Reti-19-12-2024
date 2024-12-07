/*
    File header per la gestione delle comunicazioni personalizzate su UDP con connect.
*/

#ifndef LIB_WRAPPERS_CUSTOMUDPTRANSMISSION_H
#define LIB_WRAPPERS_CUSTOMUDPTRANSMISSION_H

#include <netinet/in.h>
extern void customSend(int socket, char *buffer);
extern void customRecv(int socket, char *buffer);

#endif // LIB_WRAPPERS_CUSTOMUDPTRANSMISSION_H