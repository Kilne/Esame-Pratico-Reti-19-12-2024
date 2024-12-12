/*
    File contenente le funzioni per la gestione della griglia di gioco
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include "../wrappers/customErrorPrinting.h"

#define ROWS 11    // 10 righe di gioco + 1 riga di avviso
#define COLUMNS 20 // 20 colonne di gioco

#define EMPTY_CELL '.'
#define VOID_CELL ' '
#define METEORITE_CELL 'o'
#define SHIP_CELL '^'
#define EXPLOSION_CELL 'X'
#define DANGER_MESSAGE "METEORITE IN ARRIVO!"
#define DANGER_RANGE 3
#define LEFT_ARROW "\x1b[D"
#define RIGHT_ARROW "\x1b[C"

// Griglia di gioco 10*20
char **grid;
/*
    Posizione della navicella che si può muovere
    Solo in una riga, aggiornato a ogni iterazione
*/
int shipPosition = 9; // Posizione iniziale e centrale
// Archivio meteoriti 11*20
char **meteoritesBuffer;
int dangerMessageOn = 0;

/*
    Inizializzazione della griglia di gioco
*/
extern void init()
{
    // Allocazione 20 righe con calloc per la griglia di gioco
    grid = calloc(ROWS, sizeof(char *));

    // Allocazione delle colonne per ogni riga
    for (int i = 0; i < ROWS; i++)
    {
        grid[i] = calloc(COLUMNS, sizeof(char));
    }

    // Ogni cella della griglia è inizializzata con EMPTY_CELL tranne l'ultima riga
    for (int i = 0; i < ROWS - 1; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            grid[i][j] = EMPTY_CELL;
        }
    }

    // L'ultima riga è inizializzata con VOID_CELL
    for (int i = 0; i < COLUMNS; i++)
    {
        grid[ROWS - 1][i] = VOID_CELL;
    }

    // Posizione iniziale della nave sulla penuiltima riga
    grid[ROWS - 2][shipPosition] = SHIP_CELL;

    // Inizializzazione dell'archivio meteoriti
    // 10 righe di 20 colonne  + 1 colonna per controllo
    meteoritesBuffer = calloc(10, sizeof(char *));
    for (int i = 0; i < 10; i++)
    {
        meteoritesBuffer[i] = calloc(21, sizeof(char));
    }
    // Aggiunta marker di controllo e valori iniziali
    for (int i = 0; i < 10; i++)
    {
        for (int j = 1; j < 21; j++)
        {
            meteoritesBuffer[i][j] = '0';
        }
        // Carattere di controllo E : EMPTY quindi disponibile
        meteoritesBuffer[i][0] = 'E';
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
    // In corrispondenza della posizione della nave, se almeno 4 righe sopra
    // c'è un meteorite, viene mostrato un avviso di pericolo
    // nella riga di avviso apposita.
    int startRowCheck = ROWS - 3;                // Righa di partenza per il controllo
    int finalRowCheck = ROWS - 3 - DANGER_RANGE; // Righa finale per il controllo
    int iSdangerPassed = 0;                      // Flag per il pericolo

    // Controllo per il pericolo
    for (int i = startRowCheck; i >= finalRowCheck; i--)
    {
        // Se in una delle colonne sopra la nave c'è un meteorite si attiva il pericolo
        if (grid[i][shipPosition] == METEORITE_CELL)
        {
            iSdangerPassed = 1;
            break;
        }
    }
    if (iSdangerPassed == 1)
    {
        dangerMessageOn = 1;
    }
    else
    {
        dangerMessageOn = 0;
    }

    if (dangerMessageOn == 1)
    {
        // Se il pericolo è attivo, viene mostrato il messaggio di pericolo
        for (int i = 0; i < COLUMNS; i++)
        {
            grid[ROWS - 1][i] = DANGER_MESSAGE[i];
        }
    }
    else
    {
        // Se il pericolo non è attivo, la riga di avviso è vuota
        for (int i = 0; i < COLUMNS; i++)
        {
            grid[ROWS - 1][i] = VOID_CELL;
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
        gameOver();
        setTerminalMode(0);
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
    Funzione interna per controllare la collisione tra nave e meteorite
    @return: 0 se non c'è collisione, 1 altrimenti.
*/
int checkCollision()
{
    /*
        Controllo collisione tra la riga della nave e la riga subito sopra
        Il controllo avviene prima del "redraw" della griglia, in quanto
        si suppone che una volta la nave aveva un meteorite sopra di se e
        non si è spostata fino all'ultimo aggiornamento della griglia essa
        verrà distrutta.
    */
    for (int i = 0; i < COLUMNS; i++)
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
        // Se c'è una collisione, si attiva il game over
        for (int i = 0; i < COLUMNS; i++)
        {
            grid[ROWS - 2][i] = EXPLOSION_CELL;
        }
        // Testo di game over nella riga di avviso
        char *gameOverBuff = "GAME OVER";
        size_t len = strlen(gameOverBuff);
        for (int i = 0; i < COLUMNS; i++)
        {
            grid[ROWS - 1][i] = VOID_CELL;
        }
        for (int i = 0; i < len; i++)
        {
            grid[ROWS - 1][i] = gameOverBuff[i];
        }
        // Stampa della griglia di gioco
        printGrid();
        sleep(3);   // Attesa per la visualizzazione griglia di gioco
        gameOver(); // Funzione per il game over
        setTerminalMode(0);
        printf("Game over, uscita dal gioco a breve.\n");
        sleep(3); // Attesa per la visualizzazione del game over
        return 1; // Game over
    }
    // Avanzamento della griglia di gioco
    // Dalla righa 8 alla riga 1
    int nextRowArray = 8;
    while (nextRowArray > 0)
    {
        for (int i = 1; i < COLUMNS; i++)
        {
            grid[nextRowArray][i] = grid[nextRowArray - 1][i];
        }
        nextRowArray--;
    }
    // La riga 0 diventa vuota
    for (int i = 0; i < COLUMNS; i++)
    {
        grid[0][i] = EMPTY_CELL;
    }
    // Stampa della griglia di gioco
    printGrid();

    return 0; // Nessun game over
}
/*
    Inserimento dei meteoriti nella griglia di gioco
    @param meteoritesBuffer: buffer contenente le posizioni dei meteoriti
*/
extern void addMeteors(char *newRowBuffer)
{
    // Per dare la possibilità ai giocatori di evitare i meteoriti è necessario
    // dare uno "spazio" di manovra, quindi prima di inserire i meteoriti nella griglia
    // si genera un numero casuale tra 0 e 5 per decidere se inserire o meno i meteoriti
    srand(time(NULL));
    int insertMeteorites = rand() % 5 + 1;

    // Aggiungo a prescidere i meteoriti nell'archivio se c'è spazio
    for (int i = 0; i < 10; i++)
    {
        if (meteoritesBuffer[i][0] == 'E')
        {
            meteoritesBuffer[i][0] = 'F'; // Flag per la riga piena, FULL
            for (int j = 1; j < 21; j++)
            {
                // J-1 perché il primo carattere è il flag per l'array di archivio ma non per il buffer in arrivo
                // che è di 20 caratteri
                meteoritesBuffer[i][j] = newRowBuffer[j - 1];
            }
            break;
        }
    }

    // I meteoriti sono rappresentati da 'o' nella griglia
    // E sono inseriti sempre a partire dalla griglia 0
    // Se il numero casuale è minore di 3, non vengono inseriti meteoriti
    // Se il numero casuale è maggiore  3, vengono inseriti i meteoriti
    // Dall'archivio meteoriti
    if (insertMeteorites > 3)
    {
        int found = 0;
        while (found == 0)
        {
            // Scorro fino a trovare una F
            for (int i = 0; i < 10; i++)
            {
                if (meteoritesBuffer[i][0] == 'F')
                {
                    // Inserisco i meteoriti nella griglia
                    for (int j = 1; j < 21; j++)
                    {
                        // J-1 perché la griglia è di 20 colonne non 21
                        if (meteoritesBuffer[i][j] == '1')
                        {
                            grid[0][j - 1] = METEORITE_CELL;
                        }
                        else
                        {
                            grid[0][j - 1] = EMPTY_CELL;
                        }
                        // Al contempo svuoto l'archivio meteoriti per la riga
                        meteoritesBuffer[i][j] = '0';
                    }
                    // Flag per la riga vuota, EMPTY
                    meteoritesBuffer[i][0] = 'E';
                    found = 1; // Meteoriti inseriti, esci dal while e for
                    break;
                }
            }
        }
    }
}