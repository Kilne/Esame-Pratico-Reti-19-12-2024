/*
    File header per la gestione delle message queue IPC
*/

#ifndef LIB_WRAPPERS_CUSTOMQUEIPC_H
#define LIB_WRAPPERS_CUSTOMQUEIPC_H
#include <mqueue.h>
// Struct per i dati della coda di messaggi da usare in seguito
typedef struct
{
    char *nameOfTheQueue; // Nome della coda di messaggi
    mqd_t fileDescriptor; // Descrittore della coda di messaggi
    int error;            // Flag per l'errore
} mq_open_data;

// Creazione della coda di messaggi
extern mq_open_data createQueue(char *queueName);
// Apertura della coda di messaggi esistente
extern mq_open_data openQueue(char *queueName);
// Chiusura della coda di messaggi
extern int closeTheQueue(int mq);
// Distruzione della coda di messaggi
extern int destroyQueue(char *queueName);
// Invio di un messaggio alla coda di messaggi
extern int sendMessageToQueue(int mq, char *message);
// Ricezione di un messaggio dalla coda di messaggi
extern char *receiveMessageFromQueue(int mq);

#endif // LIB_WRAPPERS_CUSTOMQUEIPC_H
