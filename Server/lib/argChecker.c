/*
    File contenente funzioni di controllo degli argomenti
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include "wrappers/addressTools.h"

/*
    Funzione per il controllo degli argomenti
    @param argc: numero di argomenti
*/
extern void checkArgs(int argc)
{
    if (argc <2)
    {
        fprintf(stderr, "Errore: numero di argomenti errato\n");
        fprintf(stderr, "Parametri richiesti dal server per funzionare:\n  "
                        "\tLOCALHOST - per una connessione in locale\n"
                        "\tIP dotted decimal - per una connessione remota\n");
        exit(EXIT_FAILURE);
    }
}
/*
    Funzione per l'automazione nell'uso degli argomenti passati al server
    via linea di comando con @argv e impostazione della struttura dati
    per l'indirizzo del server @struct sockaddr_in
*/
extern void setTheServerAddress(char const *argv[], struct sockaddr_in *serverAddr)
{
    // Impostazione dell'indirizzo del server in base all'argomento passato
    serverAddr->sin_family = AF_INET; // IPv4 di default in ogni caso

    // Capitalizzazione stringa per confronto
    char capString[(int)strlen(argv[1])+1];
    strcpy(capString, argv[1]);
    for (int i = 0; i < (int)strlen(argv[1]); i++)
    {
        capString[i] = toupper(argv[1][i]);
    }
    // Controllo se l'argomento è LOCALHOST
    if (strcmp(capString, "LOCALHOST") == 0)
    {
        setLocalHostIPV4(&serverAddr->sin_addr);
    }
    // Controllo se l'argomento è un indirizzo IP
    // Unica notazione accettata è dotted decimal "aaa.bbb.ccc.ddd"
    else
    {
        char s[(int)strlen(argv[1])+1];
        strcpy(s, argv[1]);
        int dotCount = 0;
        int frameCount = 0;
        // Controllo dei dot
        for (int i = 0; i < (int)strlen(argv[1]); i++)
        {
            if (s[i] == '.')
            {
                dotCount++; // Contatore per i punti
            }
        }
        if (dotCount != 3)
        {
            fprintf(stderr, "Errore: indirizzo IP non valido, numero di '.' nell'indirizzo non conforme\n");
            exit(EXIT_FAILURE);
        }
        // Controllo dei numeri
        for (int i = 0; i < (int)strlen(argv[1]); i++)
        {
            if(s[i] == '.'){
                frameCount = 0;
                continue;
            }
            if (isdigit(s[i]) == 0)
            {
                fprintf(stderr, "Errore: indirizzo IP non valido, caratteri non numerici nell'indirizzo\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                frameCount++;
            }
            if (frameCount > 3)
            {
                fprintf(stderr, "Errore: indirizzo IP non valido, numero di cifre in un frame non conforme\n");
                exit(EXIT_FAILURE);
            }
        }
        // Passati i controlli imposto l'indirizzo IP
        setGenericIPV4(s, &serverAddr->sin_addr);
    }
}