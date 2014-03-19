/*
 * per compilare: gcc semaphore_to_barrier.c -Wall -lrt -O3 -o semaphore_to_barrier
 * per eseguire: ./semaphore_to_barrier
 * cosa fa: utilizzando i semafori genera una barriera riutilizzabile
 * tra processi (vedere il "piccolo libro dei semafori").
 * come funziona: il processo padre imposta una zona di memoria
 * condivisa e genera i processi figli che andranno alla barriera (per
 * il calcio di punizione). Si alternano due barriere, dove alla fine
 * dell'esecuzione di una barriera viene imposta l'altra (non sarebbe
 * stato possibile fare cio' con una sola barriera).
 * I processi a turno entrano nella regione critica tramite accesso con
 * mutex (semaforo binario) dove segnalano il loro arrivo e si bloccano
 * in attesa. L'ultimo che arriva sblocca uno dei processi arrivati
 * prima di lui che a turno sblocchera un altro processo.
 * funzioni di rilievo da vedere nel man:
 * - semafori (sem_open, sem_wait sem_post, sem_close, sem_unlink)
 * - processi (fork, wait)
 * - segnali (signal)
 * - memoria condivisa (shm_open, mmap, munmap, shm_unlink, ftruncate)
 * - varie (srand, rand, time, getpid, sleep).
 */

#include "semaphore_to_barrier.h"

// mostra il messaggio all'invio del segnale (e nulla piu')
void sig_user(int signum) {

    printf("SIGUSR1 sent...\n");
}

// costruisci una barriera riutilizzabile
int barrier_from_semaphore(void) {
    sem_t *semb, *semb_;
    sem_t *mutex;
    int *shared_int_pt;
    int sm;
    int i, k;
    pid_t my_pid;

    // crea i semafori necessari se non esistono e imposta i loro valori
    // iniziali altrimenti semplicemente aprilo per usarlo
    semb=sem_open("/semaphored_barrier",O_CREAT,S_IRUSR|S_IWUSR,0);
    semb_=sem_open("/semaphored_barrier_",O_CREAT,S_IRUSR|S_IWUSR,1);
    mutex=sem_open("/semaphored_mutex",O_CREAT,S_IRUSR|S_IWUSR,1);

    // apri il file associato alla zona di memoria condivisa dove
    // e' immagazzinato l'intero da condividere per costruire
    // la barriera
    if ((sm=shm_open("/shared_int",O_RDWR,S_IRUSR|S_IWUSR))<0) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // mappa il file di memoria condivisa nella memoria propria del del
    // processo
    shared_int_pt=(void *)mmap(NULL,sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED,sm,0);
    // credo ormai si sappia
    my_pid=getpid();
    // imposta il seme del generatore pseudocasuale
    srand(my_pid);

    // usa la barriera per N volte
    for (i=0;i<N;i++) {
        // genera un intervallo casuale (o quasi)
        k=rand()%NSEC+1;
        printf("Process %d - sleep for %d sec\n",my_pid,k);
        // dormi (e nel caso ti svegliano casualmente riaddormentati
        // finche' non e' l'ora
        while ((k=sleep(k)));
        // entra nella regione critica per utilizzare la barriera
        sem_wait(mutex);
        // incremtenta l'intero (che indica quanti processi hanno
        // raggiunto la barriera)
        (*shared_int_pt)++;
        // se sono l'ultimo processo rimasto
        if ((*shared_int_pt)==NPROC) {
            printf("Process %d - unblock the barrier\n" \
                   "********************************\n",my_pid);
            // blocca la seconda barriera cosicche' chi ci arriva non
            // passera'
            sem_wait(semb_);
            // ed esci dalla regione critica
            sem_post(mutex);
        }
        // altrimenti
        else {
            printf("Process %d - %d of %d processes remain\n",my_pid,NPROC-*shared_int_pt,NPROC);
            // esci dalla regione critica
            sem_post(mutex);
            // e attendi
            sem_wait(semb);
        }
        // sblocca la barriera per un altro processo (nel caso eri
        // l'ultimo processo che si aspettava oppure qualcuno ti ha gia'
        // sbloccato
        sem_post(semb);

        // esegui le stesse operazioni per la seconda barriera ovvero
        // entra nella regione critica relativa alla barriera
        sem_wait(mutex);
        // decrementa l'intero (che indicica ora quanti processi devono
        // ancora arrivare per sbloccare la barriera)
        (*shared_int_pt)--;
        // se non deve arrivare piu' nessuno
        if (!(*shared_int_pt)) {
            // blocca l'altra barriera, cosicche' chi ci arriva non
            // passa
            sem_wait(semb);
            // esci dalla regione critica
            sem_post(mutex);
        }
        // altrimenti
        else {
            // esci dalla regione critica
            sem_post(mutex);
            // e aspetta finche' non e' arrivato l'ultimo (ritardatario)
            sem_wait(semb_);
        }
        // rilascia uno dei processi prima di te (che a loro volta a
        // turno rilasceranno un altro processo)
        sem_post(semb_);
    }

    // rimuovi l'associazione memoria del processo corrento/file
    // condiviso
    munmap(shared_int_pt,sizeof(int));
    // imposta per l'eliminazione il file di memoria condiviso e chiudi
    // i semafori
    shm_unlink("/shared_int");
    sem_close(mutex);
    sem_close(semb);
    sem_close(semb_);
    printf("Process %d - clean done\n",my_pid);

    return 0;
}

int main(int argc, char *argv[]) {

    int nfork=NPROC;
    int i;
    pid_t ok_fork;
    pid_t my_pid;
    int w;
    int *shared_int_pt;
    int sm;

    // mostra un avviso all'arrivo del segnale
    signal(SIGUSR1,sig_user);
    // il soltio
    my_pid=getpid();
    printf("Parent %d - number of forks: %d\n",my_pid,nfork);

    // elimina i semafori se gia' esistevano (per esempio a causa di
    // una terminazione non corretta del programma)
    sem_unlink("/semaphored_barrier");
    sem_unlink("/semaphored_barrier_");
    sem_unlink("/semaphored_mutex");

    // associa un descrittore file alla memoria condivisa, creando
    // eliminando il file se esiste gia' prima di crearlo (vedi sopra)
    if ((sm=shm_open("/shared_int",O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR))<0) {
        if (errno==EEXIST) {
            shm_unlink("/shared_int");
            printf("Removing /shared_int\n");
            if ((sm=shm_open("/shared_int",O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR))<0) {
                printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
                exit(EXIT_FAILURE);
            }
        }
        else {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }
    }

    // imposta la quantita' di memoria da condividere
    if (ftruncate(sm,sizeof(int))<0)  {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // associa la memoria condivisa alla memoria effettiva del processo
    if ((shared_int_pt=(int *)mmap(NULL,sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED,sm,0))==MAP_FAILED) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // imposta il valore iniziale
    *shared_int_pt=0;

    // genera i processi figli
    for (i=0;i<nfork;i++) {
        if ((ok_fork=fork())<0) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }
        // che eseguiranno il ritrovo alla barriera
        if (!ok_fork) {
            barrier_from_semaphore();
            exit(EXIT_SUCCESS);
        }
    }
    printf("Children created\n");
    // attendi che tutti i processi figli terminino
    if (my_pid==getpid()) {
        for (i=0;i<nfork;) {
            wait(&w);
            if (WIFEXITED(w))
                i++;
        }
        printf("Children exited\n");
    }

    // rilascia le risorse
    munmap(shared_int_pt,sizeof(int));
    shm_unlink("/shared_int");
    sem_unlink("/semaphored_mutex");
    sem_unlink("/semaphored_barrier");
    sem_unlink("/semaphored_barrier_");

    printf("Cleaning done\n");
    // esci
    exit(EXIT_SUCCESS);
}
