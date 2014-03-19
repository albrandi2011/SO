/*
 * per compilare: gcc rwlock_cancel.c -Wall -lpthread -o rwlock_cancel
 * per eseguire: ./rwlock_cancel
 * cosa fa: problema dei lettori e scrittori tramite rwlock, con
 * terminazione dei thread asincrona.
 * come funziona: il thread iniziale del processo inizializza l'rwlock
 * e genera gli altri thread che si contenderanno l'oggetto e lo
 * rilasceranno ad intervalli casuali, alcuni in scrittura e altri in
 * lettura. Piu' thread in lettura possono ottenere l'accesso
 * all'oggetto rwlock, un thread soltanto in scrittura. I thread sono
 * inizializzati con l'opzione di cancellazione ascincrona, che permette
 * loro di essere cancellati immediatamente e non con l'invocazione
 * di pthread_testcancel. All'arrivo del segnale SIGUSR1 il thread
 * corrente manda lo stesso segnale agli altri thread e termina. Notare
 * che non e' possibile eseguire il rilascio dell'rwlock o azioni
 * simili. Il blocco iniziale della maschera dei segnali e la barriera
 * sono utlizzati per inizializzare in maniera sincrona i gestori dei
 * segnali.
 * funzioni di rilievo da vedere nel man:
 * - thread (pthread_create, pthread_join, pthread_self,
 * pthread_sigmask, pthread_cleanup_push, pthread_cleanup_pop,
 * pthread_setcancelstate, pthread_setcanceltype, pthread_kill)
 * - rwlock (pthread_rwlock_init, pthread_rwlock_wrlock,
 *   pthread_rwlock_rdlock, pthread_rwlock_tryrdlock,
 *   pthread_rwlock_trywrlock, pthread_rwlock_unlock,
 *   pthread_rwlock_destroy, pthread_sigmask)
 * - segnali (signal, sigprocmask, sigemptyset, sigaddset)
 * - barriere (pthread_barrier_destroy, pthread_barrier_wait,
 * pthread_barrier_init)
 * - varie (srand, rand, time, getpid, sleep).
 */

#include "rwlock_cancel.h"

volatile sig_atomic_t called=0;
pthread_rwlock_t rwlock;
pthread_t pthreadw[2];
pthread_t pthreadr[4];
pthread_barrier_t pbarrier;
sigset_t block_sig;

// esempio della funzione da chiamare all'uscita del thread, simile a
// atexit
void pthread_clean_fun(int n) {
    printf("Clean invoked by writing thread %d...\n",n);
}

// per terminare
void sig_user_exit(int signum) {
    int i;

    // se non e' gia' stato chiamato una volta l'handler
    // anche se c'e' la minima probabilita' di corse critiche
    // queste al massimo causano l'invio di qualche segnale in piu'
    if (!called) {
        printf("SIGUSR1 sent...\n");
        called=1;

        // manda il segnale agli altri thread
        // sia di lock in lettura
        for (i=0;i<4;i++)
            if (!pthread_equal(pthreadr[i],pthread_self()))
                pthread_kill(pthreadr[i],SIGUSR1);
        // che in scrittura
        for (i=0;i<2;i++)
            if (!pthread_equal(pthreadw[i],pthread_self()))
                pthread_kill(pthreadw[i],SIGUSR1);
    }
    // cerca il tuo identificativo nella lista
    // e terminati
    // sia se sono un thread lettore
    for (i=0;i<4;i++)
        if (pthread_equal(pthreadr[i],pthread_self())) {
            printf("Thread %d-r: exit...\n",i);
            pthread_cancel(pthread_self());
        }
    // che scrittore
    for (i=0;i<2;i++)
        if (pthread_equal(pthreadw[i],pthread_self())) {
            printf("Thread %d-w: exit...\n",i);
            pthread_cancel(pthread_self());
        }
}

// rwlock conteso in scrittura
void *do_something_write(long int who_i_am) {
    int i;
    int dummy;

    // inserisci la funzione da chiamare all'uscita
    pthread_cleanup_push((void (*)(void *))pthread_clean_fun,(void *)who_i_am);

    // imposta il thread come cancellabile in maniera asincrona
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&dummy);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&dummy);

    // attendi che l'abbiano fatto pure gli altri thread
    pthread_barrier_wait(&pbarrier);
    // e sblocca il segnale relativo di terminazione
    pthread_sigmask(SIG_UNBLOCK,&block_sig,NULL);

    // ripeti
    while (1) {
        // genera un in tervallo casuale
        i=rand()%9+1;
        // avvisa
        printf("Thread %ld-w: going to sleep for %d sec...\n",who_i_am,i);
        // dormi
        sleep(i);
        // prova ad acquisire il lock in scrittura
        if (pthread_rwlock_trywrlock(&rwlock)) {
            // se non ci riesci avvisa e aspetta
            printf("Thread %ld-w: wait for the rwlock...\n",who_i_am);
            pthread_rwlock_wrlock(&rwlock);
        }
        // da qui si ha il lock esclusivo
        // genera un intervallo casuale
        i=rand()%9+1;
        // avvisa
        printf("Thread %ld-w: rwlock obtained and now wait for %d sec...\n",who_i_am,i);
        // dormi
        sleep(i);
        printf("Thread %ld-w: unlock rwlock...\n",who_i_am);
        // rilascia il lock
        pthread_rwlock_unlock(&rwlock);
    }

    // nel caso di terminazione non forzata da esterni esegui la
    // funzione definita all'inizio
    pthread_cleanup_pop(1);
    return NULL;
}

// funzione per il lock in lettura
void *do_something_read(long int who_i_am) {
    int i;
    int dummy;

    // imposta il thread come cancellabile in maniera asincrona
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&dummy);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&dummy);

    // attendi che l'operazione sia compiut apure dagli altri thread
    pthread_barrier_wait(&pbarrier);
    // e sblocca la maschera
    sigprocmask(SIG_UNBLOCK,&block_sig,NULL);

    // ripeti
    while (1) {
        // genera un intervallo casuale
        i=rand()%9+1;
        // avvisa
        printf("Thread %ld-r: going to sleep for %d sec...\n",who_i_am,i);
        // dormi
        sleep(i);
        // prova ad acquisire in lock non esclusivo in lettura
        if (pthread_rwlock_tryrdlock(&rwlock)) {
            // altrimenti avvisa
            printf("Thread %ld-r: wait for the rwlock...\n",who_i_am);
            // e aspetta
            pthread_rwlock_rdlock(&rwlock);
        }
        // genera un intervallo random
        i=rand()%9+1;
        // avvisa
        printf("Thread %ld-r: rwlock obtained and now wait for %d sec...\n",who_i_am,i);
        // dormi
        sleep(i);
        printf("Thread %ld-r: unlock rwlock...\n",who_i_am);
        // rilascia il lock
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    int seed;
    long int i;

    // imposta il blocco della maschera
    sigemptyset(&block_sig);
    sigaddset(&block_sig,SIGUSR1);
    sigprocmask(SIG_BLOCK,&block_sig,NULL);

    // imposta il seme
    seed=time(NULL);
    srand(seed);

    printf("Pid:%d\n",getpid());

    // imposta l'handler
    signal(SIGUSR1,sig_user_exit);

    // alloca le risorse per l'rwlock
    pthread_rwlock_init(&rwlock,NULL);
    // e la barriera
    pthread_barrier_init(&pbarrier,NULL,6);

    // ricordati chi sei
    pthreadr[0]=pthread_self();
    // genera i thread in lettura
    for (i=1;i<4;i++)
        pthread_create(&pthreadr[i],NULL,(void *(*)(void *))do_something_read,(void *)i);
    // e in scrittura
    for (i=0;i<2;i++)
        pthread_create(&pthreadw[i],NULL,(void *(*)(void *))do_something_write,(void *)i);
    // accodati ai thread in lettura
    do_something_read(0);

    // questa parte di codice che rilascia le risorse non viene seguita
    for (i=1;i<4;i++)
        pthread_join(pthreadr[i],NULL);
    for (i=0;i<2;i++)
        pthread_join(pthreadw[i],NULL);
    pthread_rwlock_destroy(&rwlock);
    pthread_barrier_destroy(&pbarrier);

    printf("All done\n");

    exit(EXIT_SUCCESS);
}
