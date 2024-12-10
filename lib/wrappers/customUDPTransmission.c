/*
    Questo file contiene dei wrapper per le funzioni send e recv per la gestione della comunicazione
    su un socket UDP che ha utilizzato la connect per mantenere la connessione con il server.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "customErrorPrinting.h"
#include "bufHandlers.h"

/*
    Funzione che effettua la send su un socket UDP connesso.
    @param socket: il socket su cui effettuare la send
    @param buffer: il buffer da inviare
*/
extern void customSend(int socket, char *buffer)
{
    // Calcolo della dimensione del buffer + 1 per il terminatore
    // perché strlen non conta il terminatore.
    int size = strlen(buffer) + 1;

    // Invio del messaggio
    int sent = send(socket, buffer, size, 0);
    // Controllo dell'invio
    if (sent == -1)
    {
        customErrorPrinting("[ERROR] send(): la funzione ha generato un errore.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stdout, "[INFO] Inviati: %d byte\n", sent);
    }
}
/*
    Funzione che effettua la recv su un socket UDP connesso.
    @param socket: il socket su cui effettuare la recv
    @param buffer: il buffer in cui salvare il messaggio ricevuto
*/
extern void customRecv(int socket, char *buffer)
{
    // Ricezione del messaggio, size del buffer 512 come standard per la dimensione dei messaggi UDP
    int received = recv(socket, buffer, 512, 0);
    // Controllo della ricezione
    if (received == -1)
    {
        customErrorPrinting("[ERROR] recv(): la funzione ha generato un errore.\n");
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Ricevuti: %d byte\n", received);
}
