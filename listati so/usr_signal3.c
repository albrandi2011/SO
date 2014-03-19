/*
 * per compilare: gcc usr_signal3.c -Wall -O3 -o usr_signal3
 * per eseguire: ./usr_signal3
 * cosa fa: all'invio dei segnali relativi mostra un messaggio sullo
 * schermo, durante l'esecuzione determinati segnali vengono bloccati
 * come funziona: SIGUSR2 viene bloccato sia nel corpo del programma
 * che nel signal handler SIGUSR1. La maschera di blocco del signal
 * handler vale solo all'interno del signal handler, al ritorno da
 * esso viene ripristinata la maschera di blocco all'interno del
 * programma. Infatti anche dopo l'uscita dall'handler dopo la ricezione
 * di SIGUSR1 seguito da due volte invio, pur avendo effettuato lo
 * sblocco, la ricezione nel main di SIGUSR2 viene bloccata. E'
 * necessario lo sblocco successivo nel main dopo un ulteriore l'invio.
 * Attenzione, la funzione printf non e' sicura con i segnali.
 * funzioni di rilievo da vedere nel man:
 * - segnali (signal, sigemptyset, sigaddset, sigprocmask).
 */

#include "usr_signal3.h"

void sig_user_print1(int signum) {
    sigset_t block_sig;

    // inizializza l'insieme vuoto dei segnali bloccati
    sigemptyset(&block_sig);
    // aggiungi SIGUSR2
    sigaddset(&block_sig,SIGUSR2);

    // blocca SIGUSR2
    sigprocmask(SIG_BLOCK,&block_sig,NULL);
    printf("SIGUSR2 blocked in SIGUSR1\n");

    // avvisa dell'invio di SIGUSR1
    printf("SIGUSR1 sent...\n");
    printf("Press enter\n");
    // attendi invio
    getchar();


    printf("SIGUSR2 unblocked in SIGUSR1\n");
    // sblocca SIGUSR2
    sigprocmask(SIG_UNBLOCK,&block_sig,NULL);
    printf("Press enter\n");
    // attendi invio
    getchar();

    printf("Exit SIGUSR1\n");
}

void sig_user_print2(int signum) {

    // mostra l'avviso di ricezione
    printf("SIGUSR2 sent...\n");
}

int main(int argc, char *argv[]) {
    // puntatori alle funzioni di signal handler
    void (*sig_old1)(int);
    void (*sig_old2)(int);
    sigset_t block_sig;

    // inizializza l'insieme vuoto dei segnali bloccati
    sigemptyset(&block_sig);
    // aggiungi SIGUSR2
    sigaddset(&block_sig,SIGUSR2);

    // ottieni il pid da utilizzare col comando kill
    printf("My pid is: %d\n",getpid());

    // blocca SIGUSR2
    sigprocmask(SIG_BLOCK,&block_sig,NULL);
    printf("SIGUSR2 blocked outside SIGUSR1\n");

    // modifica i signal handler
    sig_old1=signal(SIGUSR1,sig_user_print1);
    sig_old2=signal(SIGUSR2,sig_user_print2);

    printf("Wait or type enter to continue...\n");
    // attendi invio
    getchar();

    printf("SIGUSR2 unblocked outside SIGUSR1\n");
    // sblocca SIGUSR2
    sigprocmask(SIG_UNBLOCK,&block_sig,NULL);

    printf("Wait or type enter to exit...\n");
    // attendi invio
    getchar();

    printf("Exit...\n");

    // ripristina i signal handler di default
    signal(SIGUSR1,sig_old1);
    signal(SIGUSR2,sig_old2);

    exit(EXIT_SUCCESS);
}
