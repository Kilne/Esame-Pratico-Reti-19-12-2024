/*
    File header per la ricezione di messaggi dal server con una funzione personalizzata
*/

#ifndef LIB_WRAPPERS_CUSTOMRECVFROM_H
#define LIB_WRAPPERS_CUSTOMRECVFROM_H

#include <netinet/in.h>
// Funzione che riceve un messaggio sul socket passato come argomento, immagazzina il messaggio in un buffer e salva
// l'indirizzo del mittente nella struttura passata come argomento.
extern void customSendTo(int socketToUse, const void *messageBuffer, const struct sockaddr *serverAddress);

#endif // LIB_WRAPPERS_CUSTOMRECVFROM_H