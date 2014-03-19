/*
 * per compilare: gcc spinlock.c -Wall -lpthread -o spinlock
 * per eseguire: ./spinlock
 * cosa fa: due thread si contendono ad intervalli di tempo casuali
 * uno spinlock. Simile al codice di esempio "mutex.c", ma utilizzando
 * uno spinlock che risulta in un'attesa attiva (osservare il tempo
 * della cpu dal monitor delle risorse).
 * come funziona: il thread iniziale del processo inizializza lo
 * spinlock e genera un secondo thread. A intervalli casuali ciascun
 * thread prova ad acquisire lo spinlock. Se ci riesce lo tiene per un
 * termpo casuale altrimenti si blocca in attesa. All'arrivo del
 * segnale SIGUSR1 viene richiesta l'uscita. Il thread che riceve il
 * segnale se dormiva si sveglia, sblocca lo spinlock se lo possedeva,
 * ed esce. Lo schedulatore passa quindi al thread restante che si
 * sblocca se aveva richiesto lo spinlock o attende che si sveglia ed
 * esce.
 * funzioni di rilievo da vedere nel man:
 * - thread (pthread_create, pthread_join)
 * - spinlock (pthread_spin_init, pthread_spin_lock,
 *   pthread_spin_unlock, pthread_spin_trylock, pthread_spin_destroy)
 * - segnali (signal)
 * - varie (srand, rand, time, getpid, sleep).
 */

#include "spinlock.h"

volatile sig_atomic_t wanna_exit;
pthread_spinlock_t spinlock;
pthread_t pthread[2];

// segnala la volonta' di terminare
void sig_user_exit(int signum) {

    printf("SIGUSR1 sent...\n");
    wanna_exit=1;
}

// funzione del thread
void *do_something(long int who_i_am) {
    int i;

    // finche' non vuoi uscire
    while (!wanna_exit) {
        i=rand()%9+1;
        // genera un intervallo casuale di tempo
        printf("Thread %ld: going to sleep for %d sec...\n",who_i_am,i);
        // se devi uscire
        if (wanna_exit) {
            printf("Thread %ld: exit...\n",who_i_am);
            // esci
            break;
        }
        // dormi per un intervallo casuale
        sleep(i);
        // se ti hanno svegliato per uscire
        if (wanna_exit) {
            printf("Thread %ld: exit...\n",who_i_am);
            // esci
            break;
        }
        // prova se lo spinlock e' libero
        if (pthread_spin_trylock(&spinlock)) {
            // in caso contrario avvisa
            printf("Thread %ld: wait for the spinlock...\n",who_i_am);
            // e aspetta
            pthread_spin_lock(&spinlock);
        }
        // da qui si ha il possesso dello spinlock
        // genera un intervallo casuale
        i=rand()%9+1;
        // esci se devi
        if (wanna_exit) {
            // non prima di aver sbloccato il mutex per l'altro processo
            pthread_spin_unlock(&spinlock);
            printf("Thread %ld: exit...\n",who_i_am);
            break;
        }
        // avvisi vari
        printf("Thread %ld: spinlock obtained and now wait for %d sec...\n",who_i_am,i);
        // dormi
        sleep(i);
        // se ti hanno svegliato per uscire
        if (wanna_exit) {
            // sblocca il mutex
            pthread_spin_unlock(&spinlock);
            printf("Thread %ld: exit...\n",who_i_am);
            // ed esci
            break;
        }
        // altrimenti rilascia il mutex
        printf("Thread %ld: unlock spinlock...\n",who_i_am);
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    int seed;

    // inizializza il seme
    seed=time(NULL);
    srand(seed);

    printf("Pid:%d\n",getpid());

    // ancora non vogliamo uscire
    wanna_exit=0;
    // imposta il signal handler
    signal(SIGUSR1,sig_user_exit);

    // inizializza lo spinlock
    pthread_spin_init(&spinlock,PTHREAD_PROCESS_PRIVATE);

    // conservati il tuo thread id (anche se qui non serve)
    pthread[0]=pthread_self();
    // crea il secondo thread
    if (pthread_create(&pthread[1],NULL,(void *(*)(void *))do_something,(void *)((long int)1)))
        printf("Error creating thread!\n");
    // vai alla funzione relativa alla gestione dello spinlock
    do_something(0);

    // se hai finito aspetta il secondo thread
    // simile al wait del fork, per evitare di produrre "thread zombie"
    // e spreco di risorse, a meno che non venga utilizzato
    // pthread_detach e il thread abbia gli attributi corretti
    pthread_join(pthread[1],NULL);
    // libera le risorse relative allo spinlock
    pthread_spin_destroy(&spinlock);

    printf("All done\n");
    // esci
    exit(EXIT_SUCCESS);
}
