/*
 * per compilare: gcc nfork.c -Wall -O3 -o nfork
 * per eseguire: ./nfork oppure ./nfork <numero di processi>
 * cosa fa: genera piu' processi secondo le istruzioni fornite da riga
 * di comando. Ogni processo figlio si identifica in base all'ordine
 * di generazione (notare che l'ordine su schermo non corrisponde, i
 * processi sono concorrenti all'accesso delle risorse). Il padre
 * attende che tutti siano terminati.
 * come funziona: il padre ottiene il numero di figli da generare come
 * secondo parametri da linea di comando, oppure in assenza genera 4
 * figli. Vengono generati i figli che mostrano alcune info e terminano.
 * Il padre aspetta che tutti i figli concludano e poi termina.
 * funzioni di rilievo da vedere nel man:
 * - espressioni regolari (fork, getpid, getppid, wait, IWEXITED)
 * - varie (argv, argc).
 */

#include "nfork.h"

int main(int argc, char *argv[]) {

    // numero di figli di default
    int nfork=4;
    int i;
    // status della fork
    pid_t ok_fork;
    // pid del padre
    pid_t my_pid;
    // info sullo status del figlio
    int w;

    // se ci sono piu' di un parametro oltre al nome del programma
    // termino
    if (argc>2) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // se e' presente il parametro relativo al numero di processi
    // da generare lo utilizzo (deve essere prima convertito da stringa
    // a intero
    if (argc==2)
        if (!sscanf(argv[1],"%d",&nfork)) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }

    printf("Number of forks: %d\n",nfork);
    // pid del padre
    my_pid=getpid();

    // genero i figli richiesti
    for (i=0;i<nfork;i++) {
        if ((ok_fork=fork())<0) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }

        // se sono il figlio mostro le informazioni relative sullo
        // schermo e termino (senza il break finale anche i figli a loro
        // volta iniziano a generare figli !!!)
        if (!ok_fork) {
            printf("I'm the %d-th child\n",i);
            break;
        }
    }

    // se sono il padre attendo che i figli terminino
    if (my_pid==getpid()) {
        printf("Done\n");
        // per un numero pari al numero dei figli
        for (i=0;i<nfork;) {
            // ottengo le informazioni sullo status di un figlio
            wait(&w);
            // se ha terminato attendo i figli rimasti
            if (WIFEXITED(w))
                i++;
        }
        printf("Really done\n");
    }

    exit(EXIT_SUCCESS);
}
