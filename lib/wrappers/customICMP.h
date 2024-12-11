/*
    File header per la funzione di controllo ICMP
*/

#ifndef LIB__WRAPPERS__CUSTOMICMP_H
#define LIB__WRAPPERS__CUSTOMICMP_H

#include <sys/socket.h>
extern int detectICMP(int socketFileDesc, struct sockaddr_in *addressToCheckAgainst);

#endif // LIB__WRAPPERS__CUSTOMICMP_H