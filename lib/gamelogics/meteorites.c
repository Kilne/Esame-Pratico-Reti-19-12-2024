/*
    Generate meteoriti casuali basati sul tempo
*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

/*
    Funzione generatrice di meteoriti casuali basata sul tempo.
    Basato sulla grandezza di una griglia di M colonne.
    @param  meteoritesBuffer: buffer in cui salvare i meteoriti
    @param  M: numero di colonne della griglia
*/
extern void generateMeteorites(char *meteoritesBuffer, int M)
{
    // Preparo un seed basato sul tempo
    srand(time(NULL));

    // Buffer di M-1 zero
    for (int i = 0; i < M - 1; i++)
    {
        meteoritesBuffer[i] = '0';
    }

    // Meteoriti generati casualmente
    int generatedMeteorites = 0;

    // Generazione dei meteoriti
    while (generatedMeteorites < 5)
    {
        int meteoritePosition = rand() % M;
        if (meteoritesBuffer[meteoritePosition] == '0')
        {
            meteoritesBuffer[meteoritePosition] = '1';
            generatedMeteorites++;
        }
    }
}