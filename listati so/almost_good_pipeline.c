/*
 * per compilare: gcc almost_good_pipeline.c -Wall -O3 -o almost_good_pipeline
 * per eseguire: ./almost_good_pipeline <primo programma con argomenti> -x- <secondo comando con argomenti>
 * (es. ./almost_good_pipeline /bin/echo ciao -x- /bin/cat)
 * cosa fa: DOVREBBE emulare il comportamento di una singola pipe "|"
 * sulla bash ("|" e' sostituito da "-x-")
 * come funziona: il padre genera una pipe che condivide col figlio. Il
 * padre e il figlio sostituiscono rispettivamente il proprio stdout e
 * stdin con i relativi ingressi della pipe. Successivamente cambiano
 * i rispettivi processi in esecuzione di modo che che il padre esegue
 * la seconda "parte" della pipe e il figlio la "prima". E' necessario
 * fornire il percorso assoluto dei programmi da eseguire. Poiche' il
 * padre esegue la seconda "parte" della pipe, termina per ultimo e non
 * ci sono problemi col terminale, ma la terminazione del figlio non
 * viene gestita correttamente poiche' con execv il processo padre
 * viene sostituito e la sua nuova immagine non contiene routine per
 * la gestione della terminazione dei figli.
 * funzioni di rilievo da vedere nel man:
 * - pipe (pipe)
 * - varie (argc, argv)
 * - file stream/descrittore file (dup2,STDIN_FILENO,STDOUT_FILENO)
 * - processi (fork, execv).
 */

#include "almost_good_pipeline.h"

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
    pid_t ok_fork;
    char **new_argv;

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

    // genera il figlio
    if ((ok_fork=fork())<0) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // se sono il padre
    if (!ok_fork) {
        // genera il nuovo argv per il primo comando
        new_argv=make_args(argv,1,k);
        // rimpiazza lo stdout con il ripsettivo ingresso della pipe
        dup2(tube[1],STDOUT_FILENO);
        // chiudi i descrittori superflui
        close(tube[0]);
        close(tube[1]);
        // sostituisci il processo corrente con quello fornito
        // dal nuovo argv
        execv(new_argv[0],new_argv);
    } else {
        // genera il nuovo argv per il secondo comando
        new_argv=make_args(argv,k+1,argc);
        // rimpiazza lo stdin con il ripsettivo ingresso della pipe
        dup2(tube[0],STDIN_FILENO);
        // chiudi i descrittori superflui
        close(tube[0]);
        close(tube[1]);
        // sostituisci il processo corrente con quello fornito
        // dal nuovo argv
        execv(new_argv[0],new_argv);
    }

    // questo codice non verra' MAI eseguito, execv non torna,
    // sostituisce il processo corrente con uno nuovo
    exit(EXIT_SUCCESS);
}
