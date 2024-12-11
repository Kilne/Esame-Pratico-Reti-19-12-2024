/*
    File per la gestione degli errori asincroni ICMO generati su un socket UDP
    Risorse utilizzate:
    - Incipit generale : GitHub Copilot
    - man7.org per : recvmsg(), ip(7), csmg, iovec, macro e funzioni per la gestione dei messaggi di errore ICMP
    - Wikipedia per : ICMP control meesages codes
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/errqueue.h> // per la struttura sock_extended_err
#include <linux/icmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "customErrorPrinting.h"

/*
    Per catturare un errore asincrono ICMP si sono fatti i seguenti step:
    1. Impostazione sul socket del flag IP_RECVERR tramite la funzione setsockopt()
    2. Ricezione del messaggio di errore ICMP tramite la funzione recvmsg() con flag IP_RECVERR
    3. Estrazione del messaggio di errore ICMP tramite la funzione cmsg_nxthdr() / csmg_firsthdr()
    4. Elaborazione del messaggio di errore ICMP per estrarre il tipo di errore e dove è avvenuto.
*/

/*
   Questa funzione permette di catturare un errore ICMP asincrono generato su un socket UDP.
   La funzione dati il file descriptor del socket e l'indirizzo del mittente, riceve il messaggio di errore ICMP
   ed elabora una risposta in base al tipo di errore ricevuto.
    @param socketFileDesc: file descriptor del socket UDP
    @param addressToCheckAgainst: indirizzo con cui confrontare l'origine del messaggio ICMP
    @return: 0 se non ci sono messaggi di errore ICMP, -1 in caso di errore, 1 se c'è un messaggio di errore ICMP
*/
extern int detectICMP(int socketFileDesc, struct sockaddr_in *addressToCheckAgainst)
{
    // Strutture dati necessarie a raccogliere i messaggi e dati accessori provenienti dal socket
    struct msghdr msg;
    struct iovec iov;
    struct cmsghdr *cmsg;

    // Strutture realtive ai nodi della rete
    struct sockaddr_in from;            // Indirizzo da cui proviene il messaggio
    struct sock_extended_err *sock_err; // Struttura per il messaggio di errore ICMP

    // Buffers per la gestione dei messaggi
    char buffer[1024];
    char control[1024];

    // Azzero le strutture
    memset(&msg, 0, sizeof(msg));
    memset(&from, 0, sizeof(from));
    memset(buffer, 0, sizeof(buffer));
    memset(control, 0, sizeof(control));

    // Set up della struttura iovec
    iov.iov_base = buffer;
    iov.iov_len = sizeof(buffer);

    // Set up della struttura msghdr per la ricezione del messaggio
    msg.msg_name = &from;
    msg.msg_namelen = sizeof(from);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    // Ricezione del messaggio di errore ICMP con FLAG MSG_ERRQUEUE che permette di accedere alla coda dove sono memorizzati
    int bytes = recvmsg(socketFileDesc, &msg, MSG_ERRQUEUE);
    if (bytes < 0)
    {
        // Se non ci sono messaggi comunicalo
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;

        customErrorPrinting("[ERROR] Errore nella ricezione del messaggio ICMP");
        return -1;
    }
    else
    {
        /*
            Se ci sono messaggi di errore li devo estrarre dalla coda di messaggi di errore del socket.
            Lo posso fare con le funzioni cmsg_firsthdr() e cmsg_nxthdr(), queste funzioni tornano un
            puntatore alla struttura cmsghdr che contiene i dati relativi al messaggio di errore.

            Creiamo un ciclo for che scorre tutti i messaggi partendo dal primo( firsthdr() ) e passando al successivo ( nxthdr() )

            Quando troviamo l'errore di nostro interesse lo mettiamo in una variabile di tipo sock_extended_err
            e ne analizziamo il contenuto per poi restituire dalla funzione la conferma di errore.
        */
        // Dal primo messaggio di errore al successivo finchè non tornano NULL
        for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg))
        {
            // Se il messaggio di errore è di tipo IP_RECVERR e il livello è SOL_IP
            if (cmsg->cmsg_level == SOL_IP && cmsg->cmsg_type == IP_RECVERR)
            {
                // Estraiamo il messaggio di errore ICMP con la funzione CMSG_DATA()
                sock_err = (struct sock_extended_err *)CMSG_DATA(cmsg);

                // Se l'origine dell'errore è un ICMP avremo un valore nel campo ee_origin pari a SO_EE_ORIGIN_ICMP(2)
                if (sock_err->ee_origin == SO_EE_ORIGIN_ICMP)
                {
                    // Copiamo l'indirizzo che orgina il messaggio ICMP in una variabile di tipo sockaddr_in
                    // dalla struttura sock_extended_err
                    struct sockaddr_in *offenderAddress = (struct sockaddr_in *)(sock_err + 1);

                    /*
                        Operiamo sulla tipologia di errore ricevuto e indirizzi.
                        Analizziamo i campi della struttura sock_extended_err per capire il tipo di errore
                        In particolare ci interessano i campi ee_type e ee_code.
                        I valori di questi due campi sono direttamente collegati ai valori nel campo
                        header del pacchetto ICMP.
                        Nello specifico ci interessano i valori realtivi al primo gruppo:
                        - type:3
                            - code:0 -> rete non raggiungibile
                            - code:1 -> host non raggiungibile
                            - code:2 -> protocollo non raggiungibile
                            - code:3 -> porta non raggiungibile
                        Altri errori non sono di nostro interesse.
                    */
                    if (sock_err->ee_type == 3)
                    {
                        if (sock_err->ee_code == 0 ||
                            sock_err->ee_code == 1 ||
                            sock_err->ee_code == 2 ||
                            sock_err->ee_code == 3)
                        {
                            // Se l'indirizzo che ha originato l'errore è uguale a quello che abbiamo passato come parametro
                            if (addressToCheckAgainst->sin_addr.s_addr == offenderAddress->sin_addr.s_addr)
                            {
                                // Stampa del messaggio di errore
                                printf("[ERROR] Errore ICMP rilevato:");
                                // Ci sono errori che non hanno una stringa di errore associata o che la funzione associata non marca con errno
                                if (strerror(sock_err->ee_errno) != NULL || strerror(sock_err->ee_errno) != "Success")
                                {
                                    printf(" %s- ", strerror(sock_err->ee_errno));
                                }
                                else
                                {
                                    printf("Flag errno non settata dalla funzione o sconosciuta- ");
                                }

                                printf("ICMP error: Tipo: %d, Codice: %d\n", sock_err->ee_type, sock_err->ee_code);
                                return 1; // Restituzione del valore di errore
                            }
                        }
                        else
                        {
                            // Se il codice dell'errore non è di nostro interesse
                            return 0;
                        }
                    }
                }
            }
        }
    }

    return 0;
}
