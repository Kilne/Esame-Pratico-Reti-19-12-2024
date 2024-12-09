/*
    File contenente le funzioni per la gestione della griglia di gioco
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include "../wrappers/customErrorPrinting.h"

#define ROWS 21 // 20 righe di gioco + 1 riga di avviso
#define COLUMNS 20

#define EMPTY_CELL ' '
#define METEORITE_CELL 'o'
#define SHIP_CELL '^'
#define EXPLOSION_CELL 'X'
#define DANGER_MESSAGE "ATTENZIONE: METEORITE IN ARRIVO!"
#define DANGER_RANGE 5
#define LEFT_ARROW "\x1b[D"
#define RIGHT_ARROW "\x1b[C"

// Griglia di gioco
char **grid;
/*
    Posizione della navicella che si può muovere
    Solo in una riga, aggiornato a ogni iterazione
*/
int shipPosition = 9; // Posizione iniziale e centrale
// Perciolo collisione attivo
int dangerActive = 0;

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

    // Inizializzazione della griglia di gioco
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if (i == ROWS - 1 && j == shipPosition)
            {
                grid[i][j] = SHIP_CELL;
            }
            else
            {
                grid[i][j] = EMPTY_CELL;
            }
        }
    }
}
/*
    Funzione per muovere la nave di gioco
*/

extern void shipMovement(int move)
{
    // La nave si muove solo in orizzontale di 1 posizione
    // tante vole quanto si preme una freccia direzionale
    // Se la nave è alla fine della griglia, non si muove
    // La posizione è tenuta in memoria per la prossima iterazione
    // di redrawRowsTicker().
    if (move == 1)
    {
        // Controllo per il movimento a destra
        if ((shipPosition + move) < (COLUMNS - 1))
        {
            grid[ROWS - 1][shipPosition] = EMPTY_CELL;
            grid[ROWS - 1][shipPosition + 1] = SHIP_CELL;
            shipPosition++; // Aggiornamento della posizione
        }
    }
    else if (move == -1)
    {
        // Controllo per il movimento a sinistra
        if ((shipPosition - move) > 0)
        {
            grid[ROWS - 1][shipPosition] = EMPTY_CELL;
            grid[ROWS - 1][shipPosition - 1] = SHIP_CELL;
            shipPosition--;
        }
    }
}
/*
    Check per avviso di pericolo
*/
void checkDanger()
{
    // Il controllo del pericolo è fatto ad oogni redrawRowsTicker()
    // In corrispondenza della posizione della nave, se almeno 5 righe sopra
    // c'è un meteorite, viene mostrato un avviso di pericolo
    // nella riga di avviso apposita.
    for (int i = ROWS - 2; i < (ROWS - 2 - DANGER_RANGE); i--)
    {
        if (grid[i][shipPosition] == METEORITE_CELL)
        {
            dangerActive = 1;
            grid[ROWS - 1] = DANGER_MESSAGE;
            break;
        }
    }
    if (dangerActive == 1) // Se il pericolo è passato, si resetta la riga di avviso
    {
        for (int i = 0; i < COLUMNS; i++)
        {
            grid[ROWS - 1][i] = EMPTY_CELL;
        }
        dangerActive = 0;
    }
}
/*
    Pulizia STDIN con escape character e tcflush per la pulizia del buffer
*/
void cleanStdin()
{
    // Pulizia visuale del terminale mediante escape character
    // il primo set di è per il posizionamento del cursore, alto sx
    // il secondo per la pulizia.
    printf("\033[H\033[J");

    // Pulizia del buffer stdin per letture successive di read
    // fatto con tcflush dalla librearia termios.h con flag TCIFLUSH
    // per la pulizia del buffer di input non letto.
    if (tcflush(STDIN_FILENO, TCIFLUSH) != 0)
    {
        customErrorPrinting("[ERROR] tcflush() fallita\n");
        exit(EXIT_FAILURE);
    }
}
/*
    Manipolazione terminale per la lettura di un singolo carattere
    @param mode: modalità di operazione del terminale[0,1]
    - 0: modalità normale (cooked)
    - 1: modalità game mode(raw)
*/
extern void setTerminalMode(int mode)
{
    // Struttura per le impostazioni del terminale
    struct termios termios_p;

    // Impostazioni del terminale corrente
    tcgetattr(STDIN_FILENO, &termios_p);

    if (mode == 0)
    {
        // Modalità normale (cooked)
        termios_p.c_lflag |= (ICANON | ECHO); // Flag per modalità canonica e echo
    }
    else if (mode == 1)
    {
        // Modalità game mode (raw)
        termios_p.c_lflag &= ~(ICANON | ECHO); // Disabilitazione di modalità canonica e echo
        termios_p.c_cc[VMIN] = 1;              // Numero di caratteri minimi per attivare la lettura
        termios_p.c_cc[VTIME] = 0;             // Timeout per la modalità non canonica
    }

    // Applicazione delle impostazioni
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);
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
    Funzione per monitorare l'input da tastiera
    e reagire ai comandi di movimento della nave,
    in aggiunta alla terminazione del gioco da parte
    dell'utente.
    @param input: input da tastiera
    @param inputSize: dimensione dell'input
*/
extern void checkTheInput(char *input, int inputSize)
{
    // Gli input essendo tutti in raw mode non hanno null terminator
    // Per fare quindi dei controlli con stringhe predefinite, si
    // deve aggiungere il null terminator manualmente.
    input[inputSize] = '\0';

    // Controllo per la terminazione del gioco

    // Controllo per il movimento della nave
    if (strcmp(input, LEFT_ARROW) == 0)
    {
        shipMovement(-1);
    }
    else if (strcmp(input, RIGHT_ARROW) == 0)
    {
        shipMovement(1);
    }
    else if (strcmp(input, "q") == 0)
    {
        setTerminalMode(0);
        cleanStdin();
        freeGrid();
        printf("Uscita dal gioco a breve.\n");
        sleep(2);
        exit(EXIT_SUCCESS);
    }
}
/*
    Stampa la griglia di gioco
*/
extern void printGrid()
{
    // Pulizia dello stdin
    cleanStdin();

    // Stampa della griglia di gioco
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
    Funzione interna per la schermata di game over
*/
void gameOver()
{
    // Ascii art per il game over
    char *gameOverText[] = {
        "  ________   __  _______",
        " / ___/ _ | /  |/  / __/",
        "/ (_ / __ |/ /|_/ / _/  ",
        "\\___/_/_|_/_/__/_/___/  ",
        " / __ \\ | / / __/ _ \\   ",
        "/ /_/ / |/ / _// , _/   ",
        "\\____/|___/___/_/|_|    ",
        "                        "};
    cleanStdin();
    for (int i = 0; i < 8; i++)
    {

        printf("%s\n", gameOverText[i]);
    }
    printf("\n");
}
/*
    Funzione interna per controllare la collisione tra la nave e un meteorite
    @param actualRow: riga attuale
    @param rowBefore: riga precedente
*/
void checkCollision(char *actualRow, char *rowBefore)
{
    int col = 0;
    while (col < COLUMNS)
    {
        // Controllo per la collisione tra nave e meteorite nella riga precedente
        // prima di aggiornare la griglia di gioco
        if (actualRow[col] == SHIP_CELL && rowBefore[col] == METEORITE_CELL)
        {
            // Game over
            gameOver();
            freeGrid();
            exit(EXIT_SUCCESS); // Uscita dal gioco
        }
        col++;
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

    /*
        Partendo dall'ultima riga, copio la riga precedente
        Facendo un controllo se la posizione della nave e un meteorite
        coincido, in quel caso la partita è persa.

        Se la posizione della nave è uguale verticalemente a quella di un meteorite
        almeno 5 righe sopra la nave, viene mostrato un avviso di pericolo.
    */
    for (int i = 0; i < COLUMNS; i++)
    {
        // Ciclo all'indietro colonna per colonna
        // ed ogni riga viene copiata nella riga successiva
        for (int j = startingRefreshRow; j == 0; j--)
        {
            checkCollision(grid[j], grid[j - 1]);
            grid[j][i] = grid[j - 1][i];
        }
    }
    // Controllo per l'avviso di pericolo
    checkDanger();
}
