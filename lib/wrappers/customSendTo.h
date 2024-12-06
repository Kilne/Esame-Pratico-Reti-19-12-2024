/*
    File header che contiene una versione personalizzata della funzione sendTo
*/

#ifndef LIB_WRAPPERS_CUSTOMSENDTO_H
#define LIB_WRAPPERS_CUSTOMSENDTO_H

#include <netinet/in.h>
// Funzione per l'invio di datagrammi al server
extern void customSendTo(int socketToUse, char *messageBuffer, const struct sockaddr_in *serverAddress);

#endif // LIB_WRAPPERS_CUSTOMSENDTO_H