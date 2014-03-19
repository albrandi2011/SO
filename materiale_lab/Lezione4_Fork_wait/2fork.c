/*
 * per compilare: gcc 2fork.c -Wall -O3 -o 2fork
 * per eseguire: ./2fork
 * cosa fa: il processo padre, buono e premuroso, genera un processo
 * figlio e attende che termina. Il figlio termina dopo aver digitato
 * 3 caratteri da tastiera (invio compreso), successivamente il padre
 * pulisce le relative risorse sullo status del figlio, attende invio
 * e termina.
 * come funziona: il figlio aspetta 3 caratteri da tastiera, il padre
 * che termini il figlio per rilasciare le risorse relative nel vettore
 * di status. Infine il padre aspetta un carattere da tastiera per
 * terminare.
 * funzioni di rilievo da vedere nel man:
 * - processi (fork, getpid, getppid, wait, WIFEXITED).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    // status di generazione del figlio
    pid_t ok_fork;
    // informazioni sullo status del figlio
    int w;

    // prova a generare il figlio
    if ((ok_fork=fork())<0) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // padre e figlio mostrano alcuni dettagli relativi a se stessi
    if (!ok_fork)
        printf("My pid is %d, my parent pid is %d, I'm the forked child\n",getpid(),getppid());
    else
        printf("My pid is %d, my parent pid is %d, my child pid is %d\n",getpid(),getppid(),ok_fork);

    // il figlio attende 3 caratteri da terminare (invio incluso)
    if (!ok_fork) {
        getchar();
        getchar();
        if (getchar()==EOF)
            printf("EOF\n");
    }

    if (!ok_fork)
        // il figlio termina
        printf("Bye bye from the child\n");
    else {
        while (1) {
            // il padre attende informazioni sullo stato del figlio
            wait(&w);
            // se il figlio ha terminato attende un carattere da
            // terminale e poi conclude altrimenti aspetta nuove
            // informazioni sullo status del figlio
            if (WIFEXITED(w)) {
                printf("Child cleaned\n");
                getchar();
                printf("Bye bye from the parent\n");
                break;
            }
        }
    }

    exit(EXIT_SUCCESS);
}