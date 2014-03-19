/*
 * per compilare: gcc spinlock_cancel.c -Wall -lpthread -o spinlock_cancel
 * per eseguire: ./spinlock_cancel
 * cosa fa: due thread si contendono ad intervalli di tempo casuali
 * uno spinlock. Simile al codice di esempio "spinlock.c", ma
 * utilizza i segnali per ottenere una terminazione asincrona ed
 * immediata, senza ritardi dovuti al sonno.
 * come funziona: il thread iniziale del processo inizializza lo
 * spinlock e genera un secondo thread. A intervalli casuali ciascun
 * thread prova ad acquisire lo spinlock. Se ci riesce lo tiene per un
 * termpo casuale altrimenti si blocca in attesa. I thread sono
 * inizializzati con l'opzione di cancellazione ascincrona, che permette
 * loro di essere cancellati immediatamente e non con l'invocazione
 * di pthread_testcancel. All'arrivo del segnale SIGUSR1 il thread
 * corrente manda lo stesso segnale all'altro thread e termina. Notare
 * che non e' possibile eseguire il rilascio dello spinlock o azioni
 * simili. Il blocco iniziale della maschera dei segnali e la barriera
 * sono utlizzati per inizializzare in maniera sincrona i gestori dei
 * segnali.
 * funzioni di rilievo da vedere nel man:
 * - thread (pthread_create, pthread_join, pthread_setcancelstate,
 *   pthread_setcanceltype, pthread_self, pthread_equal,
 *   pthread_testcancel, pthread_sigmask, pthread_kill)
 * - spinlock (pthread_spin_init, pthread_spin_lock,
 *   pthread_spin_unlock, pthread_spin_trylock, pthread_spin_destroy)
 * - segnali (signal, sigprocmask)
 * - barriere (pthread_barrier_init, pthread_barrier_wait)
 * - varie (srand, rand, time, getpid, sleep).
 */

#include "spinlock_cancel.h"

volatile sig_atomic_t called=0;
pthread_spinlock_t spinlock;
pthread_t pthread[2];
pthread_barrier_t pbarrier;
sigset_t block_sig;

// all'arrivo del segnale di uscita
void sig_user_exit(int signum) {

    // se non ero ancora stato chiamato ricordatelo
    // anche se c'e' la minima probabilita' di corse critiche
    // queste al massimo causano l'invio di qualche segnale in piu'
    if (!called) {
        printf("SIGUSR1 sent...\n");
        called=1;
    }

    // controllo quale thread sono
    if (pthread_equal(pthread[0],pthread_self())) {
        // mando il segnale pure all'altro thread
        pthread_kill(pthread[1],SIGUSR1);
        printf("Thread 0: exit...\n");
    }
    else {
        // mando il segnale pure all'altro thread
        pthread_kill(pthread[0],SIGUSR1);
        printf("Thread 1: exit...\n");
    }

    // esco immediatamente
    pthread_cancel(pthread_self());
}

// funzione realtiva allo spinlock
void *do_something(long int who_i_am) {
    int i;
    int dummy;

    // imposta la cancellazione asincrona
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&dummy);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&dummy);

    // aspetta che pure l'altro thread l'abbia fatto
    pthread_barrier_wait(&pbarrier);
    // sblocca il segnale SIGUSR1
    pthread_sigmask(SIG_UNBLOCK,&block_sig,NULL);

    while (1) {
        // selezione un intervallo casuale di tempo
        i=rand()%9+1;
        // avvisa
        printf("Thread %ld: going to sleep for %d sec...\n",who_i_am,i);
        // e dormi
        sleep(i);
        // prova ad acquisire lo spinlock
        if (pthread_spin_trylock(&spinlock)) {
            // in caso contrario avvisa
            printf("Thread %ld: wait for the spinlock...\n",who_i_am);
            // e attendi
            pthread_spin_lock(&spinlock);
        }
        // da qui ho acquisito lo spinlock
        // seleziona un intervallo casuale di tempo
        i=rand()%9+1;
        // avvisa
        printf("Thread %ld: spinlock obtained and now wait for %d sec...\n",who_i_am,i);
        // e dormi
        sleep(i);
        printf("Thread %ld: unlock spinlock...\n",who_i_am);
        // rilascia lo spinlock
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    int seed;

    // imposta la maschera per bloccare SIGUSR1
    sigemptyset(&block_sig);
    sigaddset(&block_sig,SIGUSR1);
    // blocca SIGUSR1 finche' non sono pronto per gestire correttamente
    // il segnale
    sigprocmask(SIG_BLOCK,&block_sig,NULL);

    // generazioen del seme
    seed=time(NULL);
    srand(seed);

    printf("Pid:%d\n",getpid());

    // imposta il segnale
    signal(SIGUSR1,sig_user_exit);

    // inizializza lo spinlock
    pthread_spin_init(&spinlock,PTHREAD_PROCESS_PRIVATE);
    // inizializza la barriera
    pthread_barrier_init(&pbarrier,NULL,2);

    // conserva le informazioni sul proprio thread id
    pthread[0]=pthread_self();
    // lancia l'altro thread
    if (pthread_create(&pthread[1],NULL,(void *(*)(void *))do_something,(void *)((long int)1)))
        printf("Error creating thread!\n");
    // esegui pure tu la stessa funzione dell'altro thread
    do_something(0);

    // in teoria attendi il secondo thread e libera le risorse
    // peccato che il codice qui sotto non verra mai eseguito...
    pthread_join(pthread[1],NULL);
    pthread_spin_destroy(&spinlock);
    pthread_barrier_destroy(&pbarrier);

    printf("All done\n");

    exit(EXIT_SUCCESS);
}
