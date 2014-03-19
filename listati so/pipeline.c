/*
 * per compilare: gcc pipeline.c -Wall -O3 -o pipeline
 * per eseguire: ./pipeline <primo programma con argomenti> -x- <secondo comando con argomenti>
 * (es. ./bad_pipeline /bin/echo ciao -x- /bin/less)
 * cosa fa: emula il comportamento di una singola pipe "|"
 * sulla bash ("|" e' sostituito da "-x-", in teoria si puo usare il
 * carattere di escape, i.e. "\|")
 * come funziona: il padre genera una pipe che i figli ereditano. I due
 * figli sostituiscono rispettivamente il proprio stdout e
 * stdin con i relativi ingressi della pipe. Successivamente cambiano
 * i rispettivi processi in esecuzione di modo che la pipe venga
 * eseguita. Non e' necessario fornire il percorso assoluto dei
 * programmi da eseguire. Il padre si occupa delle terminazione dei
 * figli.
 * funzioni di rilievo da vedere nel man:
 * - pipe (pipe)
 * - varie (argc, argv)
 * - file stream/descrittore file (dup2,STDIN_FILENO,STDOUT_FILENO)
 * - processi (fork, execvp).
 */

#include "pipeline.h"

// data la struttura argv (vettore di puntatori a stringhe, terminante
// con NULL), genera una nuova struttura argv che contiene un intervallo
// dell'argv originale, dove gli estremi dell'intervallo sono dati input
char **make_args(char *argv[],int l, int r) {
    char **new_argv;
    int i, j;

    // alloca lo spazio necessario tenendo pure conto del NULL finale
    new_argv=(char **)malloc(sizeof(char *)*(r-l));
    // copia il puntatore della stringa dall'argv originario a quello
    // nuovo (notare che non viene generata una nuova stringa per ogni
    // puntatore
    for (i=l,j=0;i<r;new_argv[j++]=argv[i++]);
    // inserisce il NULL finale
    new_argv[j]=NULL;

    return new_argv;
}

int main(int argc, char *argv[]) {

    int tube[2];
    int i;
    int k=0;
    pid_t ok_fork[2]={-1,-1};
    char **new_argv;
    int w;

    // per ogni parametro della line di comando
    for (i=1;i<argc;i++)
        // se corrisponde alla stringa di pipe "-x-"
        if (!strcmp(argv[i],"-x-"))
            // conserva la posizione se era la prima volta, altrimenti
            // segna l'errore (si emula solo una singola pipe)
            k=(!k)?i:-1;

    // esci se non hai trovato piu' di un simbolo di pipe "-x-"
    if (k<0) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // esci se non ci sono simboli di pipe
    if (!k) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // crea la pipe
    if (pipe(tube))  {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // genera un figlio
        if ((ok_fork[0]=fork())<0) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // se non sono il figlio genera il secondo figlio
    if (ok_fork[0])
        if ((ok_fork[1]=fork())<0) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }

    // se sono il primo figlio
    if (!(ok_fork[0])) {
        // genera il nuovo argv per il primo comando
        new_argv=make_args(argv,1,k);
        // rimpiazza lo stdout con il ripsettivo ingresso della pipe
        dup2(tube[1],STDOUT_FILENO);
        // chiudi i descrittori superflui
        close(tube[0]);
        close(tube[1]);
        // sostituisci il processo corrente con quello fornito
        // dal nuovo argv
        execvp(new_argv[0],new_argv);
    }

    // se sono il secondo figlio
    if (!(ok_fork[1])) {
        // genera il nuovo argv per il secondo comando
        new_argv=make_args(argv,k+1,argc);
        // rimpiazza lo stdout con il ripsettivo ingresso della pipe
        dup2(tube[0],STDIN_FILENO);
        // chiudi i descrittori superflui
        close(tube[0]);
        close(tube[1]);
        // sostituisci il processo corrente con quello fornito
        // dal nuovo argv
        execvp(new_argv[0],new_argv);
    }

    // se sono il padre
    if (ok_fork[0] && ok_fork[1]) {
        // chiudei i descrittori della pipe, non necessari
        close(tube[0]);
        close(tube[1]);
        // attendi che i figli abbiano terminato
        for (i=0;i<2;) {
            wait(&w);
            if (WIFEXITED(w))
                i++;
        }
        printf("All done\n");
    }

    exit(EXIT_SUCCESS);
}
