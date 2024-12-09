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
    else
    {
        /*
            TODO:
                - Se siamo nell'ipotesi che non so quanto ricevo e il
                buffer allocato è sempre di 512 byte e devo fare
                un controllo per il terminatore non posso eccedere
                la dimensione del buffer. Per cui se il messaggio
                fosse lungo 512 e senza terminatore, dovrei scartarlo.
                Al contrario se ha meno di 512 byte gli aggiungo il terminatore.
        */
        // Scarto il messaggio se non ha il terminatore ed è di 512 byte
        if (received == 512 && buffer[511] != '\0')
        {
            printf("[INFO] Messaggio scartato, dimensione 512 byte senza terminatore.\n");
            freeUDPMessage(buffer);
            buffer = getStdUDPMessage();
        }
        else if (received < 512)
        {
            // Controllo se il messaggio ha il terminatore
            if (buffer[received - 1] != '\0')
            {
                printf("[INFO] Messaggio ricevuto senza terminatore, aggiunto.\n");
                // Aggiungo il terminatore
                setUDPMessage(&buffer, buffer, received + 1);
                buffer[received] = '\0';
                        }
            else if (buffer[received - 1] == '\0')
            {
                printf("[INFO] Messaggio ricevuto minore di 512 byte.\n");
                // Ridimensiono il buffer
                setUDPMessage(&buffer, buffer, received);
            }
        }

        printf("[INFO] Ricevuti: %d byte\n", received);
    }
}