/*
 * per compilare: gcc sig_chld.c -Wall -O3 -o sig_chld
 * per eseguire: ./sig_chld
 * cosa fa: utilizza il segnale SIGCHLD per gestire la terminazione
 * dei processi figli con relativo rilascio delle risorse
 * come funziona: il padre genera dei processi figli che terminano in
 * un tempo casuale differente e attende la loro terminazione.
 * All'arrivo di SIGCHLD controlla se ci sono figli che hanno terminato
 * e rilascia le risorse. Il ciclo while e' necessario
 * poiche' mentre e' in esecuzione l'handler di SIGCHLD terminano piu'
 * di un figlio, solo un SIGCHLD risulta pendente.
 * - segnali (signal, sigemptyset, sigaddset, sigprocmask, sigsuspend)
 * - processi (waitpid, WIFEXITED, fork, getpid).
 */

#include "sig_chld.h"

// unico tipo di variabile in lettura e scrittura utilizzabile
// per interazioni tra gestori di segnali e programma a meno di non
// usare mutex et similaria, utilizzata per contare i figli vivi
// rimasti. Nota: le funzioni all'interno di un segnale deveno essere
// signal-safe e printf non lo e'
volatile sig_atomic_t sons=10;

void sig_chld(int signum) {

    int w;

    // finche' esiste un figlio che ha fornito il proprio status
    while(waitpid(WAIT_ANY,&w,WNOHANG)>0)
        // se il figlio ha terminato
        if (WIFEXITED(w))
            // decrementa il numero di figli da attendere
            sons--;
}

int main(int argc, char *argv[]) {

    int nfork=10;
    int i;
    pid_t ok_fork;
    pid_t my_pid;
    int w;
    sigset_t block_sig;
    sigset_t old_sig;

    printf("Number of forks: %d\n",nfork);
    my_pid=getpid();
    // modifica il gestore segnale
    signal(SIGCHLD,sig_chld);

    // genera i figli
    for (i=0;i<nfork;i++) {
        if ((ok_fork=fork())<0) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }

        // se sono un figlio
        if (!ok_fork) {
            // genera un intero casuale tra 1 e 10 da utilizzare come
            // tempo di attesa
            w=getpid();
            srand(w);
            w=1+rand()%10;
            printf("The %d-th child waits for %d sec\n",i,w);
            // attendi
            sleep(w);
            // esci
            exit(EXIT_SUCCESS);
        }
    }

    // inizializza la maschera di blocco dei segnali
    sigemptyset(&block_sig);
    // aggiungi SIGCHLD alla maschera
    sigaddset(&block_sig,SIGCHLD);
    // blocca SIGCHLD cosicche' se arriva tra la sospensione e la
    // lettura della variabile sons il processo non lo manchera'
    // restando in attesa perenne
    sigprocmask(SIG_BLOCK,&block_sig,&old_sig);
    // finche' ci sono figli superstiti
    while (sons) {
        // sblocca il segnale dalla maschera e contemporaneamente vai
        // in attesa di segnali. Implementato con un operazione atomica
        // di modo che se arriva un segnale tra la sospensione e la
        // lettura della variabile sons, il processo non lo manca e
        // quindi non resta in attesa perenne. Al ritorno sigsuspend
        // ripristina la maschera iniziale (cioe' con SIGCHLD bloccato)
        sigsuspend(&old_sig);
        printf("Current alive sons: %d...\n",sons);
    }
    // sblocco finale di SIGCHLD per ripristinare la maschera iniziale
    sigprocmask(SIG_UNBLOCK,&block_sig,NULL);
    printf("All done\n");

    // esci
    exit(EXIT_SUCCESS);
}

