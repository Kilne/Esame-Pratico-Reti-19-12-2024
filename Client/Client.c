/*
    Client di gioco per la comunicazione con il server
*/

#include "../lib/wrappers/basicWrappers.h"
#include "../lib/argChecker.h"
#include "../lib/wrappers/addressTools.h"
#include "../lib/wrappers/bufHandlers.h"
#include "../lib/wrappers/customUDPTransmission.h"
#include "../lib/wrappers/customConnection.h"
#include "../lib/wrappers/pollUtils.h"
#include "../lib/wrappers/customFifoTools.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>

int main(int argc, char const *argv[])
{
    // Eliminazione della FIFO se esiste
    atexit(deleteFifo);

    // Controllo degli argomenti
    checkArgs(argc);

    // Creazione del socket e opzioni per il riuso del socket
    int clientSocket = wrappedSocket(AF_INET, SOCK_DGRAM);
    wrappedSocketOpt(clientSocket);

    // Server a cui mandare e ricevere i messaggi
    struct sockaddr_in serverAddr;
    setTheServerAddress(argv, &serverAddr);
    setPortManually(&serverAddr);

    // Inizialiazzazione del polling
    startEpoll();

    // Aggiunta del socket al polling in modalità di lettura inzialmente
    addFileDescriptorToThePolling(clientSocket, EPOLLIN);

    // Impostazione della struttura per la ricezione dell'indirizzo del server da messaggi
    // ricevuti per controlli.
    struct sockaddr_in serverAddrReceived;

    /*
        Inizializzazione del buffer per la ricezione della posizione delle meteore
        è un arraya 2 dimensioni di 20x20.
    */
    int meteorStored = 0; // Contatore per i buffer delle meteore
    char **messageBuffer;
    messageBuffer = calloc(20, sizeof(char *));
    for (int i = 0; i < 20; i++)
    {
        messageBuffer[i] = calloc(20, sizeof(char));
    }

    // Special connect
    customConnection_init(clientSocket, &serverAddr);

    // Inzia il gioco
    customSend(clientSocket, "START");

/*
    TODO:
        - Implementare l'apertura del terminale                                                                 [DONE]
        - Avvviare il gioco                                                                                     [DONE]
        - Ristrutturare il buffer delle meteore in maniera tale da avere max 20 buffer da mandare al gioco      [DONE]
            - Oltre i venti buffer i messaggi vanno scartati                                                    [DONE]
            - Ogni volta che mandi una FIFO al gioco libera il buffer per un messaggio
        - Utilizzo della FIFO per la comunicazione con il gioco
        - Implementare la terminazione del gioco, con Epoll emettere un evento sulla FIFO e far chiudere il Client(previa disconnect)
            attraverso un ultimo messaggio al client per togliere la propria entry dalla lista di quelli in ascolto.
        - Indagare sulla possibile morte del server con errore ICMP e propagare l'errore al gioco ed il client,
            probabilemnte un timeout -> retry -> ICMP x 3 -> terminazione
        - Chiusura/Delete della FIFO al termine del gioco/client con atexit                                     [DONE]
*/

// Apertura del terminal con spawn di un processo figlio dedito al gioco
#ifdef __linux__
    system("gnome-terminal -- ./Game");
#elif __APPLE__
    system("open -a Terminal ./Game");
#endif

    // Ciclo di gioco
    while (1)
    {
        // Attesa degli eventi di I/O, valore arbitrario di 156
        struct epoll_event fdEvents[150];
        int triggeredEvents = waitForEvents(fdEvents, 150);

        // Controllo se il socket è pronto per la lettura
        for (int i = 0; i < triggeredEvents; i++)
        {
            if (fdEvents[i].events & EPOLLIN)
            {
                // Ricezione del messaggio
                if (meteorStored == 20)
                {
                    continue; // Scarto il messaggio
                }
                else
                {
                    // Il contatore delle meteore è anche indice di quale è il prossimo buffer da riempire
                    customRecv(clientSocket, messageBuffer[meteorStored]);
                    /*
                        TODO:
                            - Dopo aver inviato il buffer tramite FIFO liberare il buffer
                                ad operazione compiuta. e decrementare il contatore delle meteore.
                    */
                }
            }
        }
    }

    return 0;
}
