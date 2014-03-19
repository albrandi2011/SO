/*
 * per compilare: gcc cond_as_semaphore.c -Wall -lpthread -o cond_as_semaphore
 * per eseguire: ./cond_as_semaphore
 * cosa fa: utilizzo della variabile condizionale per la creazione di
 * un semaforo.
 * come funziona: il thread principale genera il resto dei thread e
 * inizializza la variabile condizionale e il mutex. Tutti i thread
 * sono generati come detached, cioe' non e' richiesto un pthread_join
 * per rilasciare le risorse alla terminazione. La variabile
 * condizionale corrisponde ad un semaforo generalizzato ed e' associata
 * a un mutex e un contatore, inizializzati dal primo thread che esegue
 * pthread_once. L'azione di wait del semaforo viene ottenuta
 * dalla combinazione dei tre oggetti sopracitati come segue: viene
 * richiesto il mutex per controllare lo stato del contatore che viene
 * decrementato di conseguenza se e' un numero non negativo con
 * successivo rilascio del mutex. Nel caso il contatore fosse a zero
 * prima del decremento il thread si sospende utilizzando la variabile
 * condizionale. In particolare in maniera atomica il processo si
 * sospende in attesa e rilascia il mutex, cosicche' non vi possono
 * essere corse critiche, ad esempio un segnale di wake tra il rilascio
 * del mutex e la sospensione. Da notare che la sospensione e' inserita
 * dentro un ciclo while poiche' a causa dell'invio di segnali possono
 * esistere risvegli non voluti. Al momento del risveglio sempre in
 * maniera atomica, il thread si sveglia e si blocca in attesa di
 * ottenere il mutex, quindi il risveglio effettivo avviene quando
 * ottiene il mutex.
 * Quando un thread rilascia una risorsa e quindi incrementa il
 * contatore deve possedere il mutex. Questo deve essere rilasciato DOPO
 * aver lanciato il segnale di wake nel caso abbia sbloccato la risorsa
 * e il contatore era a zero (cioe' qualcuno era in attesa).
 * Nel caso si rilasciassero più risorse contemporaneamente si
 * renderebbe necessario mandare un segnale di wake in broadcast.
 * La terminazione dei thread avvine non in maniera asincrona all'arrivo
 * di un segnale SIGUSR1. Il thread che gestisce il segnale manda il
 * segnale di terminazione a tutti gli altri thread e infine si suicida.
 * Se al thread e' rischiesto di terminare, la terminazione avverra'
 * soltanto durante l'esecuzione di una delle chiamate a
 * pthread_testcancel. Da notare che la terminazione effettiva viene
 * eseguita quando non si detiene il mutex.
 * funzioni di rilievo da vedere nel man:
 * - thread (pthread_create, pthread_equal, pthread_self, pthread_once,
 * pthread_setcancelstate, pthread_setcanceltype, pthread_detach,
 * phtread_sigmask, pthread_testcancel)
 * - mutex (pthread_mutex_init, pthread_mutex_lock,
 * pthread_mutex_unlock, pthread_mutex_destroy)
 * - variabile condizionale (pthread_cond_init, pthread_cond_wait,
 * pthread_cond_signal, pthread_cond_destroy)
 * - segnali (signal, sigprocmask, sigemptyset, sigaddset)
 * - barriera (pthread_barrier_wait, pthread_barrier_destroy,
 * pthread_barrier_init)
 * - varie (srand, rand, time, getpid, sleep).
 */

#include "cond_as_semaphore.h"

pthread_mutex_t mutex;
pthread_cond_t cond;
int semaphore;
pthread_t pthread[NTHREADS];
pthread_once_t whonce;
pthread_barrier_t pbarrier;
sigset_t block_sig;

// voglio uscire
void sig_user_exit(int signum) {

    int i;

    // manda il segnale di cancellazione a tutti i thread
    for (i=0;i<NTHREADS;i++)
        if (!pthread_equal(pthread[i],pthread_self())) {
            printf("Thread %d: wait to exit...\n",i);
            pthread_cancel(pthread[i]);
        }

    // ma riserva te stesso come ultimo
    for (i=0;i<NTHREADS;i++)
        if (pthread_equal(pthread[i],pthread_self())) {
            printf("Thread %d: wait to exit...\n",i);
            pthread_cancel(pthread[i]);
        }
}

// inizializzazione delle risorse
void set_sem(void) {

    semaphore=SEM_RES;
    printf("Setting semaphore...\n");
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
}

// i thread si contendono il semaforo
void *do_something(long int who_i_am) {

    int i;
    int dummy;

    // inizializza se qualcuno non l'ha ancora fatto
    pthread_once(&whonce,(void (*)(void))set_sem);
    // imposta lo stato di cancellazione non asicrona (in realta'
    // e' quello default)
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&dummy);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&dummy);
    // imposta il thread corrente come detached, cosi' non e' necessario
    // usare pthread_join per rilasciare le risorse associate al thread
    // al termine
    pthread_detach(pthread_self());

    // attendi che tutti i thread siano pronti
    pthread_barrier_wait(&pbarrier);
    // e attiva la ricezione del segnale di terminazione
    pthread_sigmask(SIG_UNBLOCK,&block_sig,NULL);

    // in eterno
    while (1) {
        // seleziona un intervallo random
        i=rand()%9+1;
        // esci se te lo hanno consigliato
        pthread_testcancel();
        printf("Thread %ld: going to sleep for %d sec...\n",who_i_am,i);
        // dormi
        sleep(i);
        // vedi di uscire se suggerito
        pthread_testcancel();
        printf("Thread %ld: try to get resources...\n",who_i_am);
        // tenta di ottenere il mutex
        pthread_mutex_lock(&mutex);
        // attendi se non ci sono risorse disponibili (il contatore
        // delle risorse e' zero)
        while (!semaphore)
            // attesa (rilascio del mutex e sospensione in maniera
            // atomica)
            pthread_cond_wait(&cond,&mutex);
        // ottiene la risorsa
        semaphore--;
        printf("Thread %ld: resources obtained - %d/%d free...\n",who_i_am,semaphore,SEM_RES);
        // e rilascia il mutex
        pthread_mutex_unlock(&mutex);
        // genera un intervallo casuale
        i=rand()%9+1;
        // esci se devi
        pthread_testcancel();
        printf("Thread %ld: doing something for %d sec with the resource...\n",who_i_am,i);
        // a nanna
        sleep(i);
        // ora ottieni il mutex
        pthread_mutex_lock(&mutex);
        // per rilasciare le risorse
        semaphore++;
        // e avvisa se c'e' qualcuno in attesa del rilascio
        pthread_cond_signal(&cond);
        printf("Thread %ld: resource released - %d/%d free...\n",who_i_am,semaphore,SEM_RES);
        // sblocca il mutex
        pthread_mutex_unlock(&mutex);
        // ed esci se devi
        pthread_testcancel();
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    int seed;
    int i;

    // blocca il segnale SIGUSR1 finche' non e' tutto impostato
    // correttamente
    sigemptyset(&block_sig);
    sigaddset(&block_sig,SIGUSR1);
    sigprocmask(SIG_BLOCK,&block_sig,NULL);

    // imposta il seme
    seed=time(NULL);
    srand(seed);

    // mostra il pid
    printf("Pid:%d\n",getpid());

    // setta l'handler del segnale
    signal(SIGUSR1,sig_user_exit);
    // inizializza la barriera
    pthread_barrier_init(&pbarrier,NULL,NTHREADS);
    // salva il tuo identificativo
    pthread[0]=pthread_self();
    // genera i thread
    for (i=1;i<NTHREADS;i++)
        if (pthread_create(&pthread[i],NULL,(void *(*)(void *))do_something,(void *)((long int)i)))
            printf("Error creating thread!\n");
    // fai quello che fanno gli altri thread
    do_something(0);

    // rilascia le risorse ed esci (tale codice non verra' mai eseguito)
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    pthread_barrier_destroy(&pbarrier);
    exit(EXIT_SUCCESS);
}
