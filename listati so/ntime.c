/*
 * per compilare: gcc ntime.c -Wall -lm -O3 -o ntime
 * per eseguire: ./ntime
 * cosa fa: permette di ottenere i tempi di cpu del processo
 * come funziona: prende il tempo all'inizio del programma (equivalente
 * al tic del matlab) e alla fine (equivalente al toc) e stampa
 * il risultato
 * funzioni di rilievo da vedere nel man:
 * - varie (times, sysconf).
 */

#include "ntime.h"

int main(int argc, char *argv[]) {

    // strutture utilizzate per conservare le informazioni sui tempi
    tictac start, stop;

    // prende il tempo iniziale
    start=gimme_clock();

    // aspetta che l'utente pressi invio per terminare
    printf("Press enter to stop...\n");
    getchar();

    // prende il tempo finale
    stop=gimme_clock();

    // calcola la differenza fra i tempi e stampa il rusultato
    elapsed_time(start,stop,VERBOSE);

    exit(EXIT_SUCCESS);
}

tictac gimme_clock(void) {
    tictac in;

    // salva il tempo corrente nella struttura tictac
    in.t=times(&in.c);
    return in;
}

stime elapsed_time(tictac start, tictac stop, int mode) {
    stime res;
    double clock_per_sec, h, m, s;

    // i tempi sono memorizzati in numero di cicli di clock
    // bisogna conoscere a quanti secondi equivale un ciclo di clock
    clock_per_sec=(double)sysconf(_SC_CLK_TCK);

    // calcola il tempo trascorso come differenza in secondi
    res.realt=(double)(stop.t-start.t)/clock_per_sec;
    res.usert=(double)(stop.c.tms_utime-start.c.tms_utime)/
        clock_per_sec;
    res.syst=(double)(stop.c.tms_stime-start.c.tms_stime)/
        clock_per_sec;

    // se richiesto stampa il risultato
    // formatta in ore:minuti:secondi
    // nota: e' anche possibile utlizzare '%' per calcolare il resto
    // di una divisione
    if (mode==VERBOSE) {
        printf("***** Elapsed  time *****\n");

        h=trunc(res.realt/(60.0*60.0));
        m=trunc((res.realt-h*60.0*60.0)/60.0);
        s=res.realt-h*60.0*60.0-m*60.0;
        printf("  Real time: %02.0f:%02.0f:%05.2f\n",h,m,s);

        h=trunc(res.usert/(60.0*60.0));
        m=trunc((res.usert-h*60.0*60.0)/60.0);
        s=res.usert-h*60.0*60.0-m*60.0;
        printf("  User time: %02.0f:%02.0f:%05.2f\n",h,m,s);

        h=trunc(res.syst/(60.0*60.0));
        m=trunc((res.syst-h*60.0*60.0)/60.0);
        s=res.syst-h*60.0*60.0-m*60.0;
        printf("System time: %02.0f:%02.0f:%05.2f\a\n",h,m,s);
    }
    return res;
}
