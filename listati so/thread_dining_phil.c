/*
 * per compilare: gcc thread_dining_phil.c -Wall -lrt -lpthread -O3 -o thread_dining_phil
 * per eseguire: ./thread_dining_phil
 * cosa fa: i filosofi a cena tramite thread.
 * come funziona: il thread principale pulisce il filesystem di
 * un'eventuale presenza di semafori lasciati da precenti esecuzioni
 * non terminate correttamente e genera i nuovi semafori da utilizzare.
 * Contrariamente a quanto avviene con l'uso dei processi non e'
 * necessario che ogni thread apra il semaforo da se', poiche' i thread
 * operano a memoria condivisa. Successivamente il thread princiaple
 * genera i processi figli (filosofi). Soltanto il thread principale
 * puo' ricevere il segnale SIGUSR1 di terminazione, in quanto per gli
 * altri thread il gestore, seppur uguale, e bloccato. All'arrivo
 * del segnale il thread principale termina i thread restanti, quindi
 * finisce l'attesa della loro terminazione, esegue le operazioni di
 * pulitura ed esce. La soluzione al problema dei filosofi a cena
 * utilizzata e quella descritta nel "piccolo libro dei semafori", NON
 * quella descritta nel Tanebaum che puo' causare starvation.
 * funzioni di rilievo da vedere nel man:
 * - processi (getpid)
 * - segnali(signal, kill, sigemptyset, sigaddset, sigprocmask)
 * - semafori (sem_open, sem_wait, sem_post, sem_close)
 * - pthread (pthread_cancel, pthread_setcancelstate,
 *   pthread_setcanceltype, pthread_create, pthread_join)
 * - barriere (pthread_barrier_wait, pthread_barrier_init,
 *   pthread_barrier_destroy)
 * - varie (sleep, rand, srand).
 */

#include "thread_dining_phil.h"

sem_t *forks[5], *table;
pthread_t pthread[NPHIL];
pthread_barrier_t pbarrier;

// arriva il segnale di uscita
void sig_user(int signum) {

    int i;

    printf("SIGUSR1 sent...\n");
    // termina tutti i figli (nota: tutti i thread condividono lo stesso
    // gestore di segnali, ma e' sbloccato solo il thread principale,
    // che quindi gestisce la terminazione)
    for (i=0;i<NPHIL;i++) {
        printf("Killing thread %d...\n",i);
        pthread_cancel(pthread[i]);
    }
}

void phil(int n) {

    int k, dummy;
    pid_t my_pid;

    // identifica il thread
    my_pid=getpid();

    // imposta lo stato di cancellazione come abilitato in maniera
    // asincrona
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&dummy);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&dummy);
    // aspetta che tutti lo siano per poter gestire la terminazione
    // correttamente
    pthread_barrier_wait(&pbarrier);

    // imposta il seme per il generatore di numeri pseudocasuale
    srand(my_pid+n);

    // la dura vita del filosofo
    while (1) {
        // genera un intervallo casuale
        k=rand()%NSEC+1;
        printf("Philosopher %d - thinking for %d sec\n",n,k);
        // e pensa (cioe' dormi), se qualcuno ti sveglia e ancora non
        // hai finito continua a pensare finche' hai esaurito la materia
        // grigia
        while ((k=sleep(k)));

        // ti e' venuta fame
        printf("Philosopher %d - want to eat\n",n);
        // prova e sederti a tavola (al massimo 4 filosofi alla volta
        // si possono sedere a tavola per tentare di mangiare evitando
        // deadlock), casomai aspetta
        sem_wait(table);

        // prova  a prendere la tua forchetta sinistra, casomai aspetta
        sem_wait(forks[LEFT(n)]);
        printf("Philosopher %d - left fork taken\n",n);
        // prova a prendere la tua forchetta destra, casomai aspetta
        sem_wait(forks[RIGHT(n)]);
        printf("Philosopher %d - right fork taken\n",n);

        k=rand()%NSEC+1;
        printf("Philosopher %d - eating for %d sec\n",n,k);
        // ora che hai entrambe le forchette mangia (cioe' dormi)
        // finche' non sei pieno
        // (a parte un filosofo, non ho mai visto nessuno mangiare con
        // due forchette)
        while ((k=sleep(k)));

        // posa la forchetta sinistra
        printf("Philosopher %d - put down left fork\n",n);
        sem_post(forks[LEFT(n)]);
        // posa la forchetta destra
        printf("Philosopher %d - put down right fork\n",n);
        sem_post(forks[RIGHT(n)]);
        // libera la tavola
        printf("Philosopher %d - finished to eat\n",n);
        sem_post(table);
    }
}

int main(int argc, char *argv[]) {

    long int i;
    pid_t my_pid;
    char philname[]="/phil_";
    sigset_t block_sig;

    // blocca il segnale SIGUSR1 (i nuovi thread erediteranno il blocco
    // sulla maschera)
    sigemptyset(&block_sig);
    sigaddset(&block_sig,SIGUSR1);
    sigprocmask(SIG_BLOCK,&block_sig,NULL);
    signal(SIGUSR1,sig_user);

    // chi sono io ? (l'origine di tutti i mali...)
    my_pid=getpid();
    printf("Parent %d - number of philosphers: %d\n",my_pid,NPHIL);

    // cancella i file associati ai semafori necessari per gestire le
    // forchette (se esistono) e crea i nuovi semafori
    for (i=0;i<NPHIL;i++) {
        philname[5]=(char)('a'+i);
        sem_unlink(philname);
        if ((forks[i]=sem_open(philname,O_CREAT|O_EXCL,S_IRUSR|S_IWUSR,1))==SEM_FAILED) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }
    }
    // come sopra per gestire il numero massimo di filosofi in tavola
    // che tentano di mangiare
    sem_unlink("/table");
    if ((table=sem_open("/table",O_CREAT|O_EXCL,S_IRUSR|S_IWUSR,NPHIL-1))==SEM_FAILED) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // inizializza la barriera
    pthread_barrier_init(&pbarrier,NULL,NPHIL+1);

    // crea i filosofi
    for (i=0;i<NPHIL;i++)
        pthread_create(&pthread[i],NULL,(void *(*)(void *))phil,(void *)i);
    // attenti che siano impostati correttamente per gestire il segnale
    // di terminazione
    pthread_barrier_wait(&pbarrier);
    // sblocca l'utilizzo del segnale di terminazione
    printf("Philosophers created\n");
    pthread_sigmask(SIG_UNBLOCK,&block_sig,NULL);

    // attenti che i filosofi muoiano (in realta' li uccidi tu stesso
    // su comando)
    for (i=0;i<NPHIL;i++) {
        pthread_join(pthread[i],NULL);
        printf("Philosopher %ld exited\n",i);
    }

    // chiudi i file associati ai semafori (l'utilizzo di sem_close per
    // chiudere subito il descrittore file associato al semaforo fa si'
    // che venga eliminato il file subito dopo con sem_unlink. Se fosse
    // invocato solo sem_unlink, la cancellazione avverrebbe non appena
    // tutti i descrittori file dei semafori sono chiusi, il che avvine
    // alla terminazione del programma
    sem_close(table);
    sem_unlink("/table");
    for (i=0;i<NPHIL;i++) {
        sem_close(forks[i]);
        philname[5]=(char)('a'+i);
        sem_unlink(philname);
    }
    // e distruggi la barriera
    pthread_barrier_destroy(&pbarrier);

    printf("Cleaning done\n");
    exit(EXIT_SUCCESS);
}
