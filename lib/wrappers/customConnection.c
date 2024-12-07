/*
    Raccolta di funzioni di utilità per la gestione delle connessioni con il server in caso di connessione UDP.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "customErrorPrinting.h"

/*
    Funzione che effettua la connect sul socket UDP per gestire l'eventualità della disconnessione
    del server.
    @param socket: il socket su cui effettuare la connect
    @param serverAddr: l'indirizzo del server a cui connettersi
*/
extern void customConnection_init(int socket, struct sockaddr_in *serverAddr)
{
    // Argomenti della connect con cast
    struct sockaddr *internal = (struct sockaddr *)serverAddr;
    socklen_t size = sizeof(*serverAddr);

    if (connect(socket, internal, size) == -1)
    {
        customErrorPrinting("[ERROR] connect(): errore nella funzione connect\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stdout, "[INFO] Connesso al server.\n");
    }
}

/*
    Funzione che effettua la disconnessione dal server.
    @param socket: il socket su cui effettuare la disconnessione
*/
extern void customDisconnect(int socket)
{
    // Preparazione di una struttura per la disconnessione con la macro AF_UNSPEC
    struct sockaddr_in disconnected;
    disconnected.sin_family = AF_UNSPEC;
    if (connect(socket, (struct sockaddr *)&disconnected, sizeof(disconnected)) == -1)
    {
        customErrorPrinting("[ERROR] disconnect(): errore nella disconnesione. \n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stdout, "[INFO] Disconnesso dal server.\n");
    }
}