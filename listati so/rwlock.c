/*
 * per compilare: gcc rwlock.c -Wall -lpthread -o rwlock
 * per eseguire: ./rwlock
 * cosa fa: problema dei lettori e scrittori tramite rwlock
 * come funziona: il thread iniziale del processo inizializza l'rwlock
 * e genera gli altri thread che si contenderanno l'oggetto e lo
 * rilasceranno ad intervalli casuali, alcuni in scrittura e altri in
 * lettura. Piu' thread in lettura possono ottenere l'accesso
 * all'oggetto rwlock, un thread soltanto in scrittura. All'arrivo del
 * segnale SIGUSR1 viene richiesta l'uscita. Il thread che riceve il
 * segnale se dormiva si sveglia, sblocca lo spinlock se lo possedeva,
 * ed esce. Lo schedulatore passa quindi ai thread restanti che si
 * sbloccano e attendono di uscire dal letargo in maniera appropriata.
 * funzioni di rilievo da vedere nel man:
 * - thread (pthread_create, pthread_join, pthread_self, pthread_detach)
 * - rwlock (pthread_rwlock_init, pthread_rwlock_wrlock,
 *   pthread_rwlock_rdlock, pthread_rwlock_tryrdlock,
 *   pthread_rwlock_trywrlock, pthread_rwlock_unlock,
 *   pthread_rwlock_destroy)
 * - segnali (signal)
 * - varie (srand, rand, time, getpid, sleep).
 */

#include "rwlock.h"

volatile sig_atomic_t wanna_exit;
pthread_rwlock_t rwlock;
pthread_t pthreadw[2];
pthread_t pthreadr[4];

// segnala la volonta' di terminare (harakiri)
void sig_user_exit(int signum) {

    printf("SIGUSR1 sent...\n");
    wanna_exit=1;
}

// rwlock conteso in scrittura
void *do_something_write(long int who_i_am) {
    int i;

    // finche' non arriva il segnale SIGUSR1 di terminazione
    while (!wanna_exit) {
        // genera un intervallo casuale
        i=rand()%9+1;
        // avviso
        printf("Thread %ld-w: going to sleep for %d sec...\n",who_i_am,i);
        // se vuoi uscire
        if (wanna_exit) {
            printf("Thread %ld-w: exit...\n",who_i_am);
            // esci
            break;
        }
        // dormi
        sleep(i);
        // se vuoi uscire
        if (wanna_exit) {
            printf("Thread %ld-w: exit...\n",who_i_am);
            // esci
            break;
        }
        // prova ad ottenere il lock in scrittura
        if (pthread_rwlock_trywrlock(&rwlock)) {
            // avvisa se non ci riesci
            printf("Thread %ld-w: wait for the rwlock...\n",who_i_am);
            // e attendi
            pthread_rwlock_wrlock(&rwlock);
        }
        // da qui ho il lock esclusivo
        // genera un numero casuale
        i=rand()%9+1;
        // se vuoi uscire
        if (wanna_exit) {
            // rilascia il lock
            pthread_rwlock_unlock(&rwlock);
            printf("Thread %ld-w: exit...\n",who_i_am);
            // ed esci
            break;
        }
        // avvisi vari
        printf("Thread %ld-w: rwlock obtained and now wait for %d sec...\n",who_i_am,i);
        // e a nanna
        sleep(i);
        // se si vuole uscire
        if (wanna_exit) {
            // rilascia il lock
            pthread_rwlock_unlock(&rwlock);
            printf("Thread %ld-w: exit...\n",who_i_am);
            // ed esci
            break;
        }
        // rilascia il lock
        printf("Thread %ld-w: unlock rwlock...\n",who_i_am);
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}

// rwlock conteso in lettura
void *do_something_read(long int who_i_am) {
    int i;

    // finche' non c'e' volonta' di uscire
    while (!wanna_exit) {
        // genera un intervallo casuale
        i=rand()%9+1;
        // avvisi
        printf("Thread %ld-r: going to sleep for %d sec...\n",who_i_am,i);
        // se vuoi uscire
        if (wanna_exit) {
            printf("Thread %ld-r: exit...\n",who_i_am);
            // esci
            break;
        }
        // dormi
        sleep(i);
        // se vuoi uscire
        if (wanna_exit) {
            printf("Thread %ld-r: exit...\n",who_i_am);
            // esci
            break;
        }
        // prova ad ottener il lock in lettura (se nessuno ci scrive)
        if (pthread_rwlock_tryrdlock(&rwlock)) {
            // avvisa in caso contrario
            printf("Thread %ld-r: wait for the rwlock...\n",who_i_am);
            // e attendi
            pthread_rwlock_rdlock(&rwlock);
        }
        // da qui abbiamo il lock
        // genera un intervallo casuale
        i=rand()%9+1;
        // se vuoi uscire
        if (wanna_exit) {
            // rilascia il lock
            pthread_rwlock_unlock(&rwlock);
            printf("Thread %ld-r: exit...\n",who_i_am);
            // ed esci
            break;
        }
        // avvisa
        printf("Thread %ld-r: rwlock obtained and now wait for %d sec...\n",who_i_am,i);
        // dormi
        sleep(i);
        // se vuoi uscire
        if (wanna_exit) {
            // rilascia il lock
            pthread_rwlock_unlock(&rwlock);
            printf("Thread %ld-r: exit...\n",who_i_am);
            // ed esci
            break;
        }
        // rilascia il lock
        printf("Thread %ld-r: unlock rwlock...\n",who_i_am);
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    int seed;
    long int i;

    // inizializza il seme del generatore pseudorandom
    seed=time(NULL);
    srand(seed);

    printf("Pid:%d\n",getpid());

    // inizializza la gestione dei segnali
    wanna_exit=0;
    signal(SIGUSR1,sig_user_exit);

    // inizializza il lock
    pthread_rwlock_init(&rwlock,NULL);

    // identificati a te stesso (in teoria qui non serve)
    pthreadr[0]=pthread_self();
    // genera i thread in lettura
    for (i=1;i<4;i++)
        pthread_create(&pthreadr[i],NULL,(void *(*)(void *))do_something_read,(void *)i);
    // genera i thread in scrittura
    for (i=0;i<2;i++)
        pthread_create(&pthreadw[i],NULL,(void *(*)(void *))do_something_write,(void *)i);
    // fai comunella con i thread in lettura
    do_something_read(0);

    // attendi che tutti abbiano finito
    // sia in lettura
    for (i=1;i<4;i++)
        pthread_join(pthreadr[i],NULL);
    // che in scrittura
    for (i=0;i<2;i++)
        pthread_join(pthreadw[i],NULL);
    // simile al wait del fork, per evitare di produrre "thread zombie"
    // e spreco di risorse, a meno che non venga utilizzato
    // pthread_detach e il thread abbia gli attributi corretti

    // libera le risorse associate al lock
    pthread_rwlock_destroy(&rwlock);

    printf("All done\n");
    // esci
    exit(EXIT_SUCCESS);
}
