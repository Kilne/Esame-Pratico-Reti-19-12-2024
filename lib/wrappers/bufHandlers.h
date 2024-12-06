/*
    File header per la gestione dei buffer di messaggi UDP
*/
#ifndef LIB_WRAPPERS_BUF_HANDLERS_H
#define LIB_WRAPPERS_BUF_HANDLERS_H
// Funzione per la creazione di un messaggio UDP di default 512 byte
extern char *getStdUDPMessage();
// Funzione per la deallocazione della memoria per il messaggio UDP
extern void freeUDPMessage(char *message);
// Funzione per inserire un messaggio nel buffer UDP
extern void setUDPMessage(char **buffer, char *message, size_t m_len);

#endif // LIB_WRAPPERS_BUF_HANDLERS_H