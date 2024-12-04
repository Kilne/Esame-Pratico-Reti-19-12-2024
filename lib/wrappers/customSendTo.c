/*
    File che contiene una versione personalizzata della funzione sendTo e
    della logica di invio di un messaggio al server.
*/

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include "customErrorPrinting.h"

/*
    Funzione che invia un messaggio al server tramite il socket passato come argomento.
    In caso di errore, stampa un messaggio di errore personalizzato. Il campo flag è
    impostato a 0.
    @socketToUse: file descriptor del socket
    @messageBuffer: buffer per il messaggio da inviare
    @serverAddress: struttura per l'indirizzo del server a cui inviare il messaggio
*/
extern void customSendTo(int socketToUse, const void *messageBuffer, const struct sockaddr *serverAddress)
{
    // Variabili per la dimensione dell'indirizzo del server e del messaggio
    socklen_t serverAddressSize = sizeof(*serverAddress);
    size_t messageSize = sizeof(messageBuffer);

    // Invio del messaggio al server
    if (sendto(socketToUse, messageBuffer, messageSize, 0, serverAddress, serverAddressSize) == -1)
    {
        customErrorPrinting("SendTo: Errore nell'invio del messaggio al server\n");
        exit(EXIT_FAILURE);
    }

    // Avviso di invio del messaggio al server in caso di successo
    printf("[INFO] Messaggio inviato al server\n");
}