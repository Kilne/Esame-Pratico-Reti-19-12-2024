/*
    Collezzione degli header per funzioni wrapper
*/

// Crea e restiuisce un valore intero per il file descriptor del socket.
extern int wrappedSocket(int domain, int type);

// Associa un indirizzo ad un socket
extern void wrappedBind(int sockfd, struct sockaddr_in *addr, socklen_t addrlen);
