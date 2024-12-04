/*
    File per la ricezione di messaggi dal server con una funzione personalizzata
    della recvFrom.
*/

#include "customErrorPrinting.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>

/*
    Funzione che riceve un messaggio dal server tramite il socket passato come argomento.
    In caso di errore, stampa un messaggio di errore personalizzato.
    @socktToRecv: file descriptor del socket
    @messageBuffer: buffer per il messaggio ricevuto
    @serverRecviedFrom: struttura per l'indirizzo del server da cui è stato ricevuto il messaggio
    @return: la lunghezza del messaggio ricevuto
*/
extern ssize_t customRecvFrom(int socktToRecv, void *messageBuffer, struct sockaddr_in *serverRecviedFrom)
{
    // Impostazione della dimensione del buffer
    size_t messageBufferSize = sizeof(messageBuffer);
    // Impostazione della dimensione dell'indirizzo del server
    struct sockaddr *serverAddr = (struct sockaddr *)serverRecviedFrom; // Cast a struct sockaddr per la recvfrom
    socklen_t serverAddrSize = sizeof(serverAddr);

    // Ricezione del messaggio
    ssize_t recvFromResult = 0;
    if (recvFromResult = recvfrom(socktToRecv, messageBuffer, messageBufferSize, 0, serverAddr, &serverAddrSize) < 0)
    {
        customErrorPrinting("recvfrom: Errore nella ricezione del messaggio\n");
        exit(EXIT_FAILURE);
    }

    // Avviso di ricezione del messaggio in caso di successo
    printf("[INFO] Messaggio ricevuto dal server\n");

    // Ritorno del risultato della recvfrom, la lunghezza del messaggio ricevuto
    // non è necessaria in quanto la recvfrom restituisce l'indiritto del mittente
    // nella struttura passata come argomento.
    return recvFromResult;
}