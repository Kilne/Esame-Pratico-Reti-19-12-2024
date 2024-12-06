/*
    Header file per la gestione degli argomenti
*/

#ifndef LIB_ARGCHECKER_H
#define LIB_ARGCHECKER_H
#include <netinet/in.h>
// Funzione per il controllo degli argomenti
extern void checkArgs(int argc);
// Funzione per l'automazione nell'uso degli argomenti passati al server
extern void setTheServerAddress(char const *argv[], struct sockaddr_in *serverAddr);
// Funzione per il controllo di porte effimere
extern void checkPort(char *port);
#endif // LIB_ARGCHECKER_H