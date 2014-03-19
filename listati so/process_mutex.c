/*
 * per compilare: gcc process_mutex.c -Wall -lpthread -lrt -o process_mutex
 * per eseguire: ./process_mutex [first | second]
 * cosa fa: due processi si contendono ad intervalli di tempo casuali
 * un mutex. I processi non sono padre e figlio.
 * come funziona: il primo processo, chiamato con ./process_mutex first,
 * controlla se la memoria condivisa mappata su file, possibilmente
 * incosistente, e' presente da una precedente esecuzione terminata non
 * correttamente, ed elimina il file. Successivamente ricrea il file per
 * la mappatura, alloca lo spazio necessario per contenere il mutex e
 * infine mappa il file nella memoria propria del processo. E' possibile
 * trovare il file creato nella directory /dev/shm/. Inizializza
 * quindi il mutex in maniera tale che possa essere condiviso tra i
 * processie alterna tra richiesta di mutex e rilascio finche' non
 * arriva il segnale di terminazione. Il secondo processo, chiamato con
 * ./pthread second, apre il file associato alla memoria condivisa e lo
 * mappa nelle sua memoria, iniziando anch'esso ad alternare tra
 * richieste del mutex e rilascio. All'arriva del segnale SIGUSR1, il
 * processo interessato, termina non dopo prima di aver rilasciato il
 * mutex se ne era in possesso e nel caso del primo processo aver
 * predisposto l'eliminazione del file di memoria condivisa.
 * funzioni di rilievo da vedere nel man:
 * - processi (fork, pthread_join)
 * - mutex (pthread_mutex_init, pthread_mutex_lock,
 *   pthread_mutex_unlock, pthread_mutex_trylock
 *   pthread_mutexattr_init, pthread_mutexattr_setpshared,
 *   pthread_mutexattr_destroy)
 * - segnali (signal)
 * - varie (srand, rand, time, getpid, sleep)
 * - memoria condivisa (shm_open, shm_unlink, ftruncate, mmap, munmap).
 */

#include "process_mutex.h"

volatile sig_atomic_t wanna_exit;

void sig_user_exit(int signum) {

    // e' arrivato il segnale di uscita
    printf("SIGUSR1 sent...\n");
    // voglio uscire
    wanna_exit=1;
}

int main(int argc, char *argv[]) {

    int seed;
    int d, i, sm;
    pthread_mutexattr_t mattr;
    pthread_mutex_t *mutex;

    // imposta il seme
    seed=time(NULL);
    srand(seed);

    // se non e' stato chiamato col numero corretto di argomenti
    // termina
    if (argc!=2) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // contralla se sei il processo da lanciare per primo
    if (!strcmp("first",argv[1])) {
        d=-1;
        printf("First process\n");
    }
    // o come secondo
    else if (!strcmp("second",argv[1])) {
        d=1;
        printf("Second process\n");
    }
    // altrimenti esci
    else  {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // ottieni il tuo pid
    printf("Pid:%d\n",getpid());

    // imposta il segnale di terminazione
    wanna_exit=0;
    signal(SIGUSR1,sig_user_exit);

    // se sei il primo processo
    if (d==-1) {
        // prova a creare il file che mappa la memoria condivisa
        if ((sm=shm_open("/shared_mtx",O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR))<0) {
            // se esiste una vecchia copia rimasta aperta
            if (errno==EEXIST) {
                // prova ad eliminarla
                shm_unlink("/shared_mtx");
                printf("Removing /shared_mtx\n");
                // e a creare il file, esci nel caso di errore
                if ((sm=shm_open("/shared_mtx",O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR))<0) {
                    printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
                    exit(EXIT_FAILURE);
                }
            }
            else {
                printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
                exit(EXIT_FAILURE);
            }
        }
    }
    // altrimenti
    else {
        // prova ad aprire il file che mappa la memoria condivisa
        if ((sm=shm_open("/shared_mtx",O_RDWR,S_IRUSR|S_IWUSR))<0) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }
    }

    // se sei il primo processo
    if (d==-1)
        // imposta le dimensioni del file/memoria condivisa per
        // contenere il mutex
        if (ftruncate(sm,sizeof(pthread_mutex_t))<0)  {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }

    // mappa il file associato alla memoria ottenura dal sistema
    // per la condivisone nel proprio spazio di memoria
    if ((mutex=(void *)mmap(NULL,sizeof(pthread_mutex_t),PROT_WRITE|PROT_READ,MAP_SHARED,sm,0))==MAP_FAILED) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }
    printf("Shared mutex obtained\n");

    // se sei il primo processo
    if (d==-1) {
        // inizializza il mutex per essere condiviso tra processi
        // e non solo tra thread
        pthread_mutexattr_init(&mattr);
        pthread_mutexattr_setpshared(&mattr,PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(mutex,&mattr);
        pthread_mutexattr_destroy(&mattr);
        printf("Mutex initialized\n");
    }

    // finche' non si deve uscire
    while (!wanna_exit) {
        // genera un intervallo casuale
        i=rand()%9+1;
        // esci se richiesto
        if (wanna_exit) {
            printf("Exit...\n");
            break;
        }
        // o vai a dormire
        printf("Going to sleep for %d sec...\n",i);
        sleep(i);
        // esci se richiesto
        if (wanna_exit) {
            printf("Exit...\n");
            break;
        }
        // prova ad ottenere il mutex
        if (pthread_mutex_trylock(mutex)) {
            printf("Wait for the mutex...\n");
            pthread_mutex_lock(mutex);
        }
        // quando l'hai ottenuto
        // genera un intervallo casuale
        i=rand()%9+1;
        // esci se richiesto dopo aver rilasciato il mutex
        if (wanna_exit) {
            pthread_mutex_unlock(mutex);
            printf("Exit...\n");
            break;
        }
        // altrimenti dormi
        printf("Mutex obtained and now wait for %d sec...\n",i);
        sleep(i);
        // se si deve uscire rilascia prima il mutex
        if (wanna_exit) {
            pthread_mutex_unlock(mutex);
            printf("Exit...\n");
            break;
        }
        // sblocca il mutex
        printf("Unlock mutex...\n");
        pthread_mutex_unlock(mutex);
    }

    // abbiamo finito e rimuoviamo l'associazione della file condiviso
    // alla memoria del processo
    munmap(mutex,sizeof(pthread_mutex_t));
    // se siamo il primo processo
    if (d==-1) {
        // impostiamo la cancellazione del file/memoria condivisa
        // (verra' effettivamente rimosso quando tutti i processi che lo
        // condividono chiuderanno il descrittore relativo (in questo
        // caso alla terminazione del processo)
        shm_unlink("/shared_mtx");
        printf("Removing shared mutex\n");
    }

    exit(EXIT_SUCCESS);
}
