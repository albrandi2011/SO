/*
 * per compilare: sig_suspend.c -Wall -O3 -o sig_suspend
 * per eseguire: ./sig_suspend
 * cosa fa: utilizza SIGALRM come una sveglia e mostra come utilizzare
 * sigsuspend e sigpending
 * come funziona: il processa imposta un allarme. Allo scadere del tempo
 * prefissato riceve il segnale SIGALRM. La chiamata atomica a
 * sigsuspend permette di attendere l'arrivo di un segnale. Il segnale
 * deve essere prima bloccato in un maschera, di modo che non arrivi
 * esattamente prima della sospensione, che in questo caso sarebbe
 * perenne. La chiamata atomica a sigsuspend va in sospensione e
 * contemporaneamente imposta la maschera di sblocco per il segnale in
 * questione. Al ripristino reimposta la maschera prima della chiamata
 * a sigsuspend (cioe' blocca di nuovo il segnale). E' possibile
 * utilizzare sigpending per vedere se ci sono segnali pendenti
 * bloccati.
 * - segnali (signal, sigemptyset, sigaddset, sigprocmask, sigsuspend)
 * - processi (waitpid, WIFEXITED, fork, getpid)
 * - varie (alarm).
 */

#include "sig_suspend.h"

// unico tipo di variabile in lettura e scrittura utilizzabile
// per interazioni tra gestori di segnali e programma a meno di non
// usare mutex et similaria. Nota: le funzioni all'interno di un segnale
// deveno essere signal-safe e printf non lo e'
volatile sig_atomic_t boom=0;

void sig_user_alarm(int signum) {

    // notifica dell'arrivo di SIGALRM
    printf("Entering SIGALRM...\n");
    printf("Boom=%d\n",boom);
    printf("Alarm expired\n");
    // cambia il valore della variabile
    boom=5;
    printf("Boom=%d\n",boom);
    printf("Exit SIGALRM...\n");
}

void sig_user_print(int signum) {

    // notifica dell'arrivo di SIGUSR1
    printf("SIGUSR1 sent...\n");
    // cambia il valore della variabile
    boom=15;
}

void sig_user_print_(int signum) {

    // notifica dell'arrivo di SIGUSR2
    printf("SIGUSR2 sent...\n");
}

int main(int argc, char *argv[]) {
    void (*sig_old)(int);
    sigset_t block_sig;
    sigset_t block_sig_;
    sigset_t old_sig;
    sigset_t test_sig;

    printf("My pid is: %d\n",getpid());

    // imposta i signal handler
    sig_old=signal(SIGALRM,sig_user_alarm);
    signal(SIGUSR1,sig_user_print);
    signal(SIGUSR2,sig_user_print_);

    printf("Setting alarm\n");
    // imposta l'allarme
    alarm(30);

    // inizializzazione della maschera di sblocco
    sigemptyset(&block_sig);
    // sospenditi in attesa si SIGALRM (se arriva un altro segnale
    // prima di SIGALRM, ad esempio SIGUSR1 o SIGUSR2, il programma
    // andra' avanti)
    sigsuspend(&block_sig);

    // stampa il valore di boom
    printf("Boom=%d\n",boom);
    printf("Something else\n");

    // reimposta l'handler originario
    signal(SIGALRM,sig_old);

    // imposta boom
    boom=0;
    printf("Boom=%d\n",boom);

    // imposta e blocca SIGUSR2
    sigemptyset(&block_sig);
    sigaddset(&block_sig,SIGUSR2);
    sigprocmask(SIG_BLOCK,&block_sig,&old_sig);
    // imposta e blocca inoltre SIGUSR1 cosicche' se arriva tra la
    // sospensione e la lettura della variabile sons il processo non lo
    // manchera' restando in attesa perenne. In old_sig e' bloccato solo
    // SIGUSR2
    sigemptyset(&block_sig_);
    sigaddset(&block_sig_,SIGUSR1);
    sigprocmask(SIG_BLOCK,&block_sig,&old_sig);
    // finche' boom e' uguale a zero (cioe' non e' stato ricevuto
    // SIGUSR1)
    while (!boom) {
        // sblocca il segnale SIGUSR1 dalla maschera e
        // contemporaneamente vai in attesa di segnali. Implementato
        // con un operazione atomica di modo che se arriva un segnale
        // tra la sospensione e la lettura della variabile boom, il
        // processo non lo manca e quindi non resta in attesa perenne.
        // Al ritorno sigsuspend ripristina la maschera iniziale
        // (cioe' con SIGUSR1 bloccato). Nota la ricezione di SIGUSR2 e'
        // bloccata
        sigsuspend(&old_sig);
        // guarda i segnali pendenti bloccati
        sigpending(&test_sig);
        // se era stato inviato SIGUSR2 (prima di SIGUSR1, o di un altro
        // segnale diverso da SIGUSR2, poiche' e' bloccato)
        if (sigismember(&test_sig,SIGUSR2))
            // avvisami
            printf("SIGUSR2 pending...\n");
    }
    // sblocco finale di SIGUSR1 per ripristinare la maschera iniziale
    sigprocmask(SIG_UNBLOCK,&block_sig_,NULL);

    printf("Boom=%d\nDone\n",boom);

    exit(EXIT_SUCCESS);
}
