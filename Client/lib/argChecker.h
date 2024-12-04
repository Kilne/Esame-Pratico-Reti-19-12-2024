/*
    Header file per la gestione degli argomenti
*/

#ifndef SERVER_LIB_ARGCHECKER_H
#define SERVER_LIB_ARGCHECKER_H

// Funzione per il controllo degli argomenti
extern void checkArgs(int argc);
// Funzione per l'automazione nell'uso degli argomenti passati al server
extern void setTheServerAddress(char const *argv[], struct sockaddr_in *serverAddr);
#endif // SERVER_LIB_ARGCHECKER_H