/*
 * per compilare: gcc mutex.c -Wall -lpthread -o mutex
 * per eseguire: ./mutex
 * cosa fa: due thread si contendono ad intervalli di tempo casuali
 * un mutex.
 * come funziona: il thread iniziale del processo inizializza il mutex
 * e genera un secondo thread. A intervalli casuali ciascun thread prova
 * ad acquisire il mutex. Se ci riesce lo tiene per un termpo casuale
 * altrimenti si blocca in attesa. All'arrivo del segnale SIGUSR1
 * viene richiesta l'uscita. Il thread che riceve il segnale se
 * dormiva si sveglia, sblocca il mutex se lo possedeva, ed esce. Lo
 * schedulatore passa quindi al thread restante che si sblocca se aveva
 * richiesto il mutex o attende che si sveglia ed esce.
 * funzioni di rilievo da vedere nel man:
 * - thread (pthread_create, pthread_join)
 * - mutex (pthread_mutex_init, pthread_mutex_lock,
 *   pthread_mutex_unlock, pthread_mutex_trylock, pthread_mutex_destroy)
 * - segnali (signal)
 * - varie (srand, rand, time, getpid, sleep).
 */

#include "mutex.h"

volatile sig_atomic_t wanna_exit;
pthread_mutex_t mutex;
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
        // genera un intervallo casuale di tempo
        i=rand()%9+1;
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
        // prova se il mutex e' libero
        if (pthread_mutex_trylock(&mutex)) {
            // in caso contrario avvisa
            printf("Thread %ld: wait for the mutex...\n",who_i_am);
            // e aspetta
            pthread_mutex_lock(&mutex);
        }
        // da qui si ha il possesso del mutex
        // genera un intervallo casuale
        i=rand()%9+1;
        // esci se devi
        if (wanna_exit) {
            // non prima di aver sbloccato il mutex per l'altro processo
            pthread_mutex_unlock(&mutex);
            printf("Thread %ld: exit...\n",who_i_am);
            break;
        }
        // avvisi vari
        printf("Thread %ld: mutex obtained and now wait for %d sec...\n",who_i_am,i);
        // dormi
        sleep(i);
        // se ti hanno svegliato per uscire
        if (wanna_exit) {
            // sblocca il mutex
            pthread_mutex_unlock(&mutex);
            printf("Thread %ld: exit...\n",who_i_am);
            // ed esci
            break;
        }
        // altrimenti rilascia il mutex
        printf("Thread %ld: unlock mutex...\n",who_i_am);
        pthread_mutex_unlock(&mutex);
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

    // inizializza il mutex (in quanto allocato staticamente era pure
    // possibile usare la macro PTHREAD_MUTEX_INITIALIZER)
    pthread_mutex_init(&mutex,NULL);

    // conservati il tuo thread id (anche se qui non serve)
    pthread[0]=pthread_self();
    // crea il secondo thread
    if (pthread_create(&pthread[1],NULL,(void *(*)(void *))do_something,(void *)(long int)1))
        printf("Error creating thread!\n");
    // vai alla funzione relativa alla gestione del mutex
    do_something(0);

    // se hai finito aspetta il secondo thread
    pthread_join(pthread[1],NULL);
    // libera le risorse relative al mutex
    pthread_mutex_destroy(&mutex);

    printf("All done\n");
    // esci
    exit(EXIT_SUCCESS);
}
