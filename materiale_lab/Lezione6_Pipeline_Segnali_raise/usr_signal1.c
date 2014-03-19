/*
 * per compilare: gcc usr_signal.c -Wall -O3 -o usr_signal
 * per eseguire: ./usr_signal
 * cosa fa: all'invio dei segnali relativi mostra un messaggio sullo
 * schermo
 * come funziona: dopo l'esecuzione del comando viene inviato il segnale
 * SIGUSR1 al programma (kill -s SIGUSR1 <pid>). Se si inviano altri
 * segnali SIGUSR1 finche' il signal handler relativo non termina
 * tramite invio seguito da "bg <numero job>" (il segnale di stop
 * SIGSTOP e' invocato dallo stesso processo con raise), questi non
 * vengono eseguiti ma restano sospesi. Dopo aver pressato l'invio
 * comunque il gestore verra' eseguito soltanto una volta, anche se
 * sono stati ricevuti piu' di un segnale SIGUSR1. La ricezione di
 * SIGUSR2 (kill -s SIGUSR2 <pid>), segnale differente, invece
 * interrompe il signal handler di SIGUSR1. Attenzione, la funzione
 * printf non e' sicura con i segnali.
 * funzioni di rilievo da vedere nel man:
 * - segnali (signal, raise).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void sig_user_print1(int signum);
void sig_user_print2(int signum);
int main(int argc, char *argv[]);


// SIGUSR1 signal handler
void sig_user_print1(int signum) {

    // avverti che e' arrivato il segnale
    printf("SIGUSR1 sent...\n");
    printf("Press enter\n");
    // attendi un carattere (invio)
    getchar();

    // invia a te stesso SIGSTOP (bg <numero job> per continuare)
    raise(SIGSTOP);

    // segnala l'uscita dall'handler
    printf("Exit SIGUSR1\n");
}

// SIGUSR1 signal handler
void sig_user_print2(int signum) {

    // avverti che e' arrivato il segnale
    printf("SIGUSR2 sent...\n");
    printf("Press enter\n");
    // attendi un carattere (invio)
    getchar();

    // segnala l'uscita dall'handler
    printf("Exit SIGUSR2\n");
}

int main(int argc, char *argv[]) {
    // puntatori alle funzioni dei signal handler
    void (*sig_old1)(int);
    void (*sig_old2)(int);

    // stampa il pid da utilizzare col kill
    printf("My pid is: %d\n",getpid());

    // sostituisci i signal handler di default
    // (che terminano il processo)
    sig_old1=signal(SIGUSR1,sig_user_print1);
    sig_old2=signal(SIGUSR2,sig_user_print2);

    printf("Wait or type enter to exit...\n");
    //attendi invio
    getchar();

    // rimetti i signal handler di default
    signal(SIGUSR1,sig_old1);
    signal(SIGUSR2,sig_old2);

    exit(EXIT_SUCCESS);
}
