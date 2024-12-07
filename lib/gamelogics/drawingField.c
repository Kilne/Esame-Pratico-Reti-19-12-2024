/*
 */

#include <stdio.h>
#include <stdlib.h>

#define ROWS 21 // 20 righe di gioco + 1 riga di avviso
#define COLUMNS 20

#define EMPTY_CELL ' '
#define METEORITE_CELL 'o'
#define SHIP_CELL '^'
#define DANGER_MESSAGE "ATTENZIONE: METEORITE IN ARRIVO!"
#define DANGER_RANGE 5

// Griglia di gioco
char **grid;
/*
    Posizione della navicella che si può muovere
    Solo in una riga, aggiornato a ogni iterazione
*/
int shipPosition = 9; // Posizione iniziale e centrale

/*
    Inizializzazione della griglia di gioco
*/
extern void init()
{
    // Allocazione della memoria per la griglia di gioco
    grid = calloc(ROWS, sizeof(char *));

    for (int i = 0; i < ROWS; i++)
    {
        grid[i] = calloc(COLUMNS, sizeof(char));
    }
}

/*
    Stampa la griglia di gioco
*/
extern void printGrid()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            printf("%c", grid[i][j]);
        }
        printf("\n");
    }
}

/*
    Rimozione della grid di gioco e deallocazione della memoria
*/
extern void freeGrid()
{
    for (int i = 0; i < ROWS; i++)
    {
        free(grid[i]);
    }
    free(grid);
    shipPosition = 9;
}

/*
    Inserimento dei meteoriti nella griglia di gioco
    @param meteoritesBuffer: buffer contenente le posizioni dei meteoriti
*/
extern void addMeteors(char *newRowBuffer)
{
    // I meteoriti sono rappresentati da 'o' nella griglia
    // E sono inseriti sempre a partire dalla griglia 0
    for (int i = 0; i < COLUMNS; i++)
    {
        if (newRowBuffer[i] == '1')
        {
            grid[0][i] = METEORITE_CELL;
        }
        else
        {
            grid[0][i] = EMPTY_CELL;
        }
    }
}

/*
    Scorre la griglia di gioco verso il basso
*/
extern void redrawRowsTicker()
{
    // Partendo dalla penultima riga, scorro verso il basso
    // Riga di avviso esclusa
    int startingRefreshRow = ROWS - 2;
    while (startingRefreshRow > 0)
    {
        /*
            Partendo dall'ultima riga, copio la riga precedente
            Facendo un controllo se la posizione della nave e un meteorite
            coincido, in quel caso la partita è persa.

            Se la posizione della nave è uguale verticalemente a quella di un meteorite
            almeno 5 righe sopra la nave, viene mostrato un avviso di pericolo.
        */
        for (int i = 0; i < COLUMNS; i++)
        {
            // TODO: FARE REDRAW
            // TODO: FARE CONTROLLO COLLISIONE
            // TODO: FARE GAME OVER LOGIC
        }
    }
}