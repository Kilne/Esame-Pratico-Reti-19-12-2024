/*
    File contenente le funzioni per la gestione della griglia di gioco
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include "../wrappers/customErrorPrinting.h"

#define ROWS 11    // 10 righe di gioco + 1 riga di avviso
#define COLUMNS 20 // 20 colonne di gioco

#define EMPTY_CELL '.'
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
// Prossimo avanzamento della griglia di gioco
int nextRow = 1;

/*
    Inizializzazione della griglia di gioco
*/
extern void init()
{
    // Allocazione 20 righe con calloc per la griglia di gioco
    grid = calloc(ROWS, sizeof(char *));

    // Allocazione delle colonne per ogni riga
    for (int i = 0; i <= ROWS; i++)
    {
        grid[i] = calloc(COLUMNS, sizeof(char));
    }

    // Ogni cella della griglia è inizializzata con EMPTY_CELL
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            grid[i][j] = EMPTY_CELL;
        }
    }

    // Posizione iniziale della nave
    grid[ROWS - 2][shipPosition] = SHIP_CELL;
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
        if ((shipPosition + move) < COLUMNS)
        {
            grid[ROWS - 2][shipPosition] = EMPTY_CELL;
            grid[ROWS - 2][shipPosition + 1] = SHIP_CELL;
            shipPosition++;
        }
    }
    else if (move == -1)
    {
        // Controllo per il movimento a sinistra
        if ((shipPosition - move) > 1)
        {
            grid[ROWS - 2][shipPosition] = EMPTY_CELL;
            grid[ROWS - 2][shipPosition - 1] = SHIP_CELL;
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
    int firstRowToCheck = ROWS - 3;                      // Riga 18 sopra la nave
    int lastRowToCheck = firstRowToCheck - DANGER_RANGE; // Riga 13 sopra la nave
    int danger = 0;
    for (int i = firstRowToCheck; i < lastRowToCheck; i--)
    {
        if (grid[i][shipPosition] == METEORITE_CELL)
        {
            danger = 1;
            break;
        }
    }
    dangerActive = danger;
    // Se c'è pericolo, viene mostrato l'avviso
    if (dangerActive == 1)
    {
        for (int i = 0; i < strlen(DANGER_MESSAGE); i++)
        {
            grid[ROWS - 1][i] = DANGER_MESSAGE[i];
        }
    }
    else
    {
        for (int i = 1; i < COLUMNS; i++)
        {
            grid[ROWS - 1][i] = EMPTY_CELL;
        }
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
    Funzione per monitorare l'input da tastiera
    e reagire ai comandi di movimento della nave,
    in aggiunta alla terminazione del gioco da parte
    dell'utente.
    @param input: input da tastiera
    @param inputSize: dimensione dell'input
    @return: 0 se l'input non è di chiusura del gioco, 1 altrimenti.
*/
extern int checkTheInput(char *input, int inputSize)
{
    // Gli input essendo tutti in raw mode non hanno null terminator
    // Per fare quindi dei controlli con stringhe predefinite, si
    // deve aggiungere il null terminator manualmente.
    input[inputSize] = '\0';

    // Controllo tipo di input ricevuto
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
        gameOver();
        printf("Uscita dal gioco a breve.\n");
        return 1; // Chiusura del gioco
    }
    return 0; // Nessuna chiusura del gioco
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
    for (int i = 1; i < COLUMNS; i++)
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
    Funzione interna per controllare la collisione tra nave e meteorite
*/
int checkCollision()
{
    // Controllo collisione tra la riga della nave e la riga subito sopra
    for (int i = 1; i < COLUMNS; i++)
    {
        if (grid[ROWS - 2][i] == SHIP_CELL && grid[ROWS - 3][i] == METEORITE_CELL)
        {
            return 1; // Collisione
        }
    }
    return 0; // Nessuna collisione
}
/*
    Scorre la griglia di gioco verso il basso
    @return: 0 se non c'è game over, 1 altrimenti.
*/
extern int redrawRowsTicker()
{
    // Controllo per l'avviso di pericolo
    checkDanger();
    // Controllo per la collisione tra nave e meteorite
    if (checkCollision() == 1)
    {
        setTerminalMode(0);
        gameOver();
        printf("Game over, uscita dal gioco a breve.\n");
        return 1; // Game over
    }
    // Avanzamento della griglia di gioco
    for (int i = nextRow; i > 0; i--)
    {
        for (int j = 1; j < COLUMNS; j++)
        {
            // Copia della riga superiore in quella inferiore
            grid[i][j] = grid[i - 1][j];
            // Pulizia della riga superiore
            grid[i - 1][j] = EMPTY_CELL;
        }
    }
    nextRow++; // Incremento della prossima riga
    if (nextRow == ROWS - 1)
    {
        nextRow--;
    }
    // Stampa della griglia di gioco
    printGrid();

    return 0; // Nessun game over
}
