/*
 * per compilare: gcc 2fork_messy.c -Wall -O3 -o 2fork_messy
 * per eseguire: ./2fork_messy
 * cosa NON fa: DOVREBBE generare un processo figlio che termina dopo
 * che invio e' stato premuto 2 volte, ma termina dopo il primo invio
 * (quando digitate il secondo invio siete gia' sulla shell, potete
 * provare a digitare un comando e funzionera')
 * come NON funziona: il padre termina prima del figlio, pertanto
 * la shell (ancestor, i.e. padre del padre) non sapendo del nipote
 * come puo' (dop il primo invio), toglie lo stdin e quindi le succesive
 * richieste di input da terminale danno EOF=-1
 * funzioni di rilievo da vedere nel man:
 * - processi (fork, getpid, getppid).
 */

#include "2fork_messy.h"

int main(int argc, char *argv[]) {

    // conserva lo stato di padre/figlio dopo il fork
    pid_t ok_fork;

    // prova a generare il figlio
    if ((ok_fork=fork())<0) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // stampa info sul processo corrente
    if (!ok_fork)
        // sono il figlio
        printf("My pid is %d, my parent pid is %d, I'm the forked child\n",getpid(),getppid());
    else
        // sono il padre
        printf("My pid is %d, my parent pid is %d, my child pid is %d\n",getpid(),getppid(),ok_fork);

    // se sono il filgio attendi 3 caratteri da terminare
    // arrivera' solo il primo
    if (!ok_fork) {
        printf("%d ",getchar());
        printf("%d ",getchar());
        if (getchar()==EOF)
            printf("EOF\n");
    }

    // salutare per educazione
    if (!ok_fork)
        // il figlio saluta
        printf("Bye bye from the child\n");
    else
        // il padre saluta
        printf("Bye bye from the parent\n");

    exit(EXIT_SUCCESS);
}
