/*
    Visual representation of the game field
*/

#ifndef LIB_GAMELOGICS_DRAWINGFIELD_H
#define LIB_GAMELOGICS_DRAWINGFIELD_H

// Inizializzazione della griglia di gioco
extern void init();
// Stampa la griglia di gioco
extern void printGrid();
// Rimozione della grid di gioco e deallocazione della memoria
extern void freeGrid();
// Aggiunta dei meteoriti alla griglia di gioco
extern void addMeteors(char *newRowBuffer);
// Scorrimento delle righe della griglia di gioco verso il basso
extern void redrawRowsTicker();
// Movimento della nave di gioco
extern void shipMovement(int move);
// Modifica del terminale per la lettura dei tasti freccia
extern void setTerminalMode(int mode);
// Controllo dell'input da tastiera
extern void checkTheInput(char *input, int inputSize);

#endif // LIB_GAMELOGICS_DRAWINGFIELD_H
