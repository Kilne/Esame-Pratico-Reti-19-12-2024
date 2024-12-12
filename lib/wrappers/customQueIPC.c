/*
    Wrapper per implementare la comunicazione tra i processi tramite code di messaggi IPC.

    - Richiesto kernel Linux 2.6.6 o superiore.
    - https://www.man7.org/linux/man-pages/man7/mq_overview.7.html
*/
#define _GNU_SOURCE // Per la definizione  strerrorname_np()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <mqueue.h>
#include "customErrorPrinting.h"
#include "customQueIPC.h"
#include <errno.h>
#include <time.h>
#include <unistd.h>

/*
    Crea un coda di messaggi con il nome passato come parametro.
    @return: una struttura dati che raccoglie i risultati della creazione della coda.
*/
extern mq_open_data createQueue(char *queueName)
{
    /*
        Creazione della coda di messaggi.
        Flag:
        - O_RDWR: apertura in lettura/scrittura
        - O_NONBLOCK: apertura in lettura non bloccante
        - O_CREAT: crea la coda se non esiste
        - O_EXCL: se la coda esiste già ritorna errore
        - 0666: permessi di lettura e scrittura per tutti
        - NULL: attributi di default della coda per il sistema
    */
    mq_open_data data; // Struttura per i dati della coda di messaggi
    data.nameOfTheQueue = queueName;
    data.fileDescriptor = -1;
    data.error = -1;

    mqd_t mq = mq_open(data.nameOfTheQueue, O_RDWR | O_NONBLOCK | O_CREAT | O_EXCL, 0666, NULL);
    if (mq == (mqd_t)-1)
    {
        // Errore nella creazione della coda di messaggi, già esistente
        if (errno == EEXIST)
        {
            // Ne modifico il nome
            printf("[ERROR] mq_open(): Coda %s già esistente\n", data.nameOfTheQueue);
            srand(time(NULL));                                                                     // Inizializzazione del generatore di numeri casuali
            char *newQueueName = calloc(20, sizeof(char));                                         // 20 caratteri per il nome della coda
            sprintf(newQueueName, "%s%d", data.nameOfTheQueue, rand() % 999);                      // Aggiunta di un numero casuale al nome
            data.nameOfTheQueue = newQueueName;                                                    // Aggiornamento del nome della coda
            mq = mq_open(data.nameOfTheQueue, O_RDWR | O_NONBLOCK | O_CREAT | O_EXCL, 0666, NULL); // Creazione della coda con il nuovo nome
            if (mq == (mqd_t)-1)
            {
                printf("[ERROR] mq_open(): Errore creazione della coda %s: \n%s\n", strerrorname_np(errno), strerrordesc_np(errno));
                return data;
            }
            else
            {
                data.error = 0;
                data.fileDescriptor = mq;
                return data;
            }
        }
        else
        {
            printf("[ERROR] mq_open(): Errore creazione della coda %s: \n%s\n", strerrorname_np(errno), strerrordesc_np(errno));
            return data;
        }
    }
    // Coda creata con successo
    data.error = 0;
    data.fileDescriptor = mq;
    return data;
}
/*
    Funzione per aprire una coda di messaggi esistente.
    @param queueName: nome della coda da aprire
    @return: una struttura dati che raccoglie i risultati dell'apertura della coda.
*/
extern mq_open_data openQueue(char *queueName)
{
    mq_open_data data; // Struttura per i dati della coda di messaggi
    data.nameOfTheQueue = queueName;
    data.fileDescriptor = -1;
    data.error = -1;

    mqd_t mq = mq_open(data.nameOfTheQueue, O_RDWR | O_NONBLOCK, NULL);
    if (mq == (mqd_t)-1)
    {
        printf("[ERROR] mq_open(): Errore apertura della coda %s: \n%s\n", strerrorname_np(errno), strerrordesc_np(errno));
        return data;
    }
    // Coda aperta con successo
    data.error = 0;
    data.fileDescriptor = mq;
    return data;
}
/*
    Chiude la coda di messaggi passata come parametro.
    @return: 0 in caso di successo, -1 in caso di errore.
*/
extern int closeTheQueue(int mq)
{
    if (mq_close(mq) == -1)
    {
        customErrorPrinting("[ERROR] mq_close(): Errore chiusura della coda\n");
        return -1;
    }
    return 0;
}
/*
    Funzione per la distruzione della coda
    @param queueName: nome della coda da distruggere
    @return: 0 in caso di successo, -1 in caso di errore.
*/
extern int destroyQueue(char *queueName)
{
    if (mq_unlink(queueName) == -1)
    {
        customErrorPrinting("[ERROR] mq_unlink(): Errore distruzione della coda\n");
        return -1;
    }
    return 0;
}
/*
    Invio di un messaggio alla coda di messaggi passata come parametro.
    @param mq: descrittore della coda di messaggi
    @param message: messaggio da inviare
    @return: 0 in caso di successo, 1 in caso di coda piena, -1 in caso di errore.
 */
extern int sendMessageToQueue(int mq, char *message)
{
    if (mq_send(mq, message, strlen(message), 0) == -1)
    {
        if (errno == EAGAIN)
        {
            // Coda piena per evitare il blocco del thread si è abilitata la lettura non bloccante
            // Da gestire al di fuori della funzione
            customErrorPrinting("[ERROR] mq_send(): Coda piena\n");
            return 1;
        }
        customErrorPrinting("[ERROR] mq_send(): Errore invio messaggio\n");
        return -1;
    }
    return 0;
}
/*
    Funzione per la ricezione dei messaggi dalla coda di messaggi passata come parametro.
    @param mq: descrittore della coda di messaggi
    @return: il messaggio ricevuto, NULL in caso di errore, "EMPTY" in caso di coda piena.
 */
extern char *receiveMessageFromQueue(int mq)
{
    char *buffer = calloc(8192, sizeof(char));
    if (mq_receive(mq, buffer, 8192, NULL) == -1)
    {
        if (errno == EAGAIN)
        {
            // Coda vuota, il blocco del thread è evitato con la flag O_NONBLOCK
            customErrorPrinting("[ERROR] mq_receive(): Coda vuota\n");
            return "EMPTY";
        }
        customErrorPrinting("[ERROR] mq_receive(): Errore ricezione messaggio\n");
        return NULL;
    }
    return buffer;
}
