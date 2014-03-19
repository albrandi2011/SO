/*
 * per compilare: gcc 2fork_zombie.c -Wall -O3 -o 2fork_zombie
 * per eseguire: ./2fork_zombie
 * cosa fa: il processo padre, sacerdote voodoo, genera un processo
 * figlio e lo transforma in processo zombie. Dicesi processo zombie
 * un processo figlio terminato, di cui le informazioni di status non
 * sono state ancora raccolte dal padre e quindi occupano risorse nel
 * vettore di status del sistema. Quando il padre muore, tutti i
 * processi figli zombie e non, detti orfani, vengono adottati dal
 * processo init (id 1), il capostipite di ogni cosa, che in caso di
 * zombie provvedera' al rilascio delle risorse occupate.
 * come NON funziona: il padre aspetta input da tastiera mentre il
 * figlio termina. Prima di terminare il padre, e possibile vedere lo
 * stato zombie del figlio da
 * "Sistema-->Amministrazione-->Monitor di sitema". Successivamente
 * dopo che il padre termina, il figlio zombie verra' adottato dal
 * processo init e le sue risorse rilasciate e quindi non sara' piu'
 * presente.
 * funzioni di rilievo da vedere nel man:
 * - processi (fork, getpid, getppid).
 */

#include "2fork_zombie.h"

int main(int argc, char *argv[]) {

    pid_t ok_fork;

    // genera il figlio
    if ((ok_fork=fork())<0) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // sia il padre che il figlio mostrano alcune loro informazioni
    // relative
    if (!ok_fork)
        printf("My pid is %d, my parent pid is %d, I'm the forked child\n",getpid(),getppid());
    else
        printf("My pid is %d, my parent pid is %d, my child pid is %d\n",getpid(),getppid(),ok_fork);

    // il padre si ferma in attesa di input
    if (ok_fork) {
        getchar();
    }

    if (!ok_fork)
        // il figlio termina e diventa zombi
        printf("Bye bye from the child\n");
    else
        // il padre termina
        printf("Bye bye from the parent\n");

    exit(EXIT_SUCCESS);
}
