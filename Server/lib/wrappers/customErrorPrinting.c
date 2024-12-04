/*
    File contenente una funzione custom che combina perror e strerror
    per la stampa degli errori, basati su errno.
*/

#include <errno.h>
#include <string.h>
#include <stdio.h>

/*
    Funzione per la stampa degli errori customizzata
    @message: messaggio da stampare
*/
extern void customErrorPrinting(char *message)
{
    perror(message);
    if (errno != 0)
    {
        strerror(errno);
    }
}