/*
    Client di gioco per la comunicazione con il server
*/

#include "../lib/wrappers/basicWrappers.h"
#include "../lib/argChecker.h"
#include "../lib/wrappers/addressTools.h"
#include "../lib/wrappers/bufHandlers.h"
#include "../lib/wrappers/customUDPTransmission.h"
#include "../lib/wrappers/customErrorPrinting.h"
#include "../lib/wrappers/customConnection.h"
#include "../lib/wrappers/pollUtils.h"
#include "../lib/wrappers/customQueIPC.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define GAME_QUEUE "/gameQueue"     // Coda per messaggi CLIENT -> GAME
#define CLIENT_QUEUE "/clientQueue" // Coda per messaggi GAME -> CLIENT

int main(int argc, char const *argv[])
{

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
        è un array a 2 dimensioni di 20 righe. Teoricamente il client non dovrebbe
        sapere a priori la dimensione del buffer in arrivo ( standard UDP a 512 byte)
        e dovrebbe essere in grado di riallocare la memoria per il buffer in arrivo.
    */
    int meteorStored = -1; // Contatore per i buffer delle meteore parte da meno 1 per l'array 0 based
    // Per scelta di design del gioco massimo 20 buffer di meteore
    char **messageBuffer = calloc(20, sizeof(char *));

    // Buffer per la ricezione dei messaggi dalla FIFO
    char *bufferReceived = calloc(20, sizeof(char));

    // Registrzione del client con il server a livello Kernel
    customConnection_init(clientSocket, &serverAddr);

    // Inizializzazione della message queue per la comunicazione con il gioco
    mq_open_data queueData = createQueue(GAME_QUEUE);
    mq_open_data queueData_2 = createQueue(CLIENT_QUEUE);
    int queFd = queueData.fileDescriptor;            // File descriptor della coda client -> gioco
    char *queueName = queueData.nameOfTheQueue;      // Nome della coda, da passare poi come argomento al gioco
    int queFd_2 = queueData_2.fileDescriptor;        // File descriptor della coda gioco -> client
    char *queueName_2 = queueData_2.nameOfTheQueue;  // Nome della coda, da passare poi come argomento al gioco
    addFileDescriptorToThePolling(queFd, EPOLLOUT);  // Scrivo nella coda al gioco
    addFileDescriptorToThePolling(queFd_2, EPOLLIN); // Leggo dalla coda al client

    // Segnale per la terminazione del gioco
    int gameTerminated = 0;
    // Stringa di controllo chiusura del gioco
    char *gameEndString = "GAME OVER";

    // Inzia il gioco
    int sendRes = customSend(clientSocket, &serverAddr, "START");
    if (sendRes == 1)
    {
        customErrorPrinting("[ERROR] Il server non è raggiungibile\n");
        exit(EXIT_FAILURE);
    }

    // Apertura del terminal con spawn di un processo figlio dedito al gioco
    int systemResult = 0;
    char *baseCommand = "gnome-terminal -- ./Game";
    // Preparazione del comando per l'apertura del terminale con argomenti i nomi delle code
    char *composedCommand = calloc(strlen(baseCommand) +
                                       strlen(queueName) +
                                       strlen(queueName_2) +
                                       2 + 1,
                                   sizeof(char)); // 2 spazi e terminatore
    sprintf(composedCommand, "%s %s %s", baseCommand, queueName, queueName_2);
    systemResult = system(composedCommand);
    if (systemResult == -1)
    {
        customErrorPrinting("[ERROR] Apertura del terminale fallita\n");
        exit(EXIT_FAILURE);
    }
    // Valore temporale per generare un ping al server ogni 30 secondi
    double generatePingAfter = 30;
    time_t currentTimeElapsed;
    time_t timeFromLastPing;
    time(&timeFromLastPing);

    // Ciclo di gioco
    while (gameTerminated == 0)
    {
        // Ping al server per controllare la connessione ogni 30 secondi
        time(&currentTimeElapsed);
        if (difftime(currentTimeElapsed, timeFromLastPing) >= generatePingAfter)
        {
            int pingRes = customSend(clientSocket, &serverAddr, "PING");
            if (pingRes == 1)
            {
                customErrorPrinting("[ERROR] Il server non è raggiungibile\n");
                // Chiusura del gioco
                gameTerminated = 1;
                int s = sendMessageToQueue(queFd, "DEAD");
                printf("[INFO] Chiusura del gioco\n");
                continue;
            }
            time(&timeFromLastPing);
        }
        else
        {
            currentTimeElapsed = 0;
        }

        // Attesa degli eventi di I/O
        struct epoll_event fdEvents[10];
        int triggeredEvents = waitForEvents(fdEvents, 10);
        // Logica evento per evento
        for (int i = 0; i < triggeredEvents; i++)
        {
            // Controllo se il socket è pronto per la lettura
            if (fdEvents[i].data.fd == clientSocket)
            {
                // Ricezione del messaggio
                // Per scelta di design del gioco il client scarta i messaggi se ha già 20 buffer
                if (meteorStored != 19)
                {
                    // Incremento il contatore delle meteore PRIMA di ricevere il buffer perchè è 0 based
                    meteorStored++;
                    // Se posso ricevere il buffer, lo ricevo allocando un buffer standard UDP
                    // alla posizione meteorStored
                    messageBuffer[meteorStored] = getStdUDPMessage();

                    // Il contatore delle meteore è anche indice di quale è il buffer da riempire
                    customRecv(clientSocket, messageBuffer[meteorStored]);
                    printf("[INFO] Ricevuto buffer dal server: %s\n", messageBuffer[meteorStored]);
                }
            }
            // Controllo se la coda è pronta per operazioni di I/O
            else if (fdEvents[i].data.fd == queFd)
            {
                // EVENTO DI SCRITTURA
                if (fdEvents[i].events & EPOLLOUT)
                {
                    // Invio del buffer alla FIFO
                    int messageSent = 0;
                    if (meteorStored != -1) // Controllo se ci sono buffer da inviare
                    {
                        // RAGIONEVOLMENTE non dovrei passare array nulli alla coda con il contatore
                        messageSent = sendMessageToQueue(queFd, messageBuffer[meteorStored]);
                        if (messageSent == 0)
                        {
                            printf("[INFO] Inviati dati alla coda: %s\n", messageBuffer[meteorStored]);
                        }
                        else if (messageSent == 1)
                        {
                            // Ritento per 5 volte l'invio del messaggio
                            int retryCounter = 0;
                            while (retryCounter < 5)
                            {
                                messageSent = sendMessageToQueue(queFd, messageBuffer[meteorStored]);
                                if (messageSent == 0)
                                {
                                    printf("[INFO] Inviati dati alla coda: %s\n", messageBuffer[meteorStored]);
                                    break;
                                }
                                else if (messageSent == -1)
                                {
                                    customErrorPrinting("[ERROR] Errore nell'invio del messaggio alla coda\n");
                                    break;
                                }
                                retryCounter++;
                            }
                            // Teoricamente la coda non dovrebbe riempiersi mai
                            customErrorPrinting("[ERROR] Coda piena, impossibile inviare il messaggio\n");
                        }
                        else
                        {
                            customErrorPrinting("[ERROR] Errore nell'invio del messaggio alla coda\n");
                        }
                        // Libero il buffer per il prossimo messaggio
                        freeUDPMessage(messageBuffer[meteorStored]);
                        // Decremento il contatore delle meteore
                        meteorStored--;
                    }
                }
            }
            else if (fdEvents[i].data.fd == queFd_2)
            {
                // EVENTO DI LETTURA
                if (fdEvents[i].events & EPOLLIN)
                {
                    // Ricezione del messaggio dalla coda
                    char *messageReceived = receiveMessageFromQueue(queFd_2);
                    if (messageReceived != NULL)
                    {
                        // Controllo se il messaggio ricevuto è di chiusura del gioco
                        if (strcmp(messageReceived, gameEndString) == 0)
                        {
                            // Il gioco è stato chiuso
                            gameTerminated = 1;
                            break; // Uscita dal ciclo for
                        }
                    }
                    else if (strcmp(messageReceived, "EMPTY") == 0)
                    {
                        customErrorPrinting("[INFO] Coda vuota\n");
                    }
                    else
                    {
                        customErrorPrinting("[ERROR] Errore nella ricezione del messaggio dalla coda\n");
                    }
                    free(messageReceived);
                }
            }
        }
    }

    if (gameTerminated == 1)
    {

        // Chiusura dell'istanza di epoll
        removeFileDescriptorFromThePolling(clientSocket);
        removeFileDescriptorFromThePolling(queFd);
        closeEpoll();

        // Chiusura della message queue
        closeTheQueue(queFd);
        closeTheQueue(queFd_2);
        destroyQueue(queueData.nameOfTheQueue);
        destroyQueue(queueData_2.nameOfTheQueue);

        // Deregistrazione del client dal server
        customDisconnect(clientSocket);

        // Chiusura del socket
        if (close(clientSocket) == -1)
        {
            customErrorPrinting("[ERROR] close(): Errore nella chiusura del socket\n");
            exit(EXIT_FAILURE);
        }

        // Liberazione della
        free(messageBuffer);
        free(bufferReceived);

        // Uscita dal client
        exit(EXIT_SUCCESS);
    }

    return 0;
}
