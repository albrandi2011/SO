/*
 * per compilare: gcc dining_phil.c -Wall -lrt -O3 -o dining_phil
 * per eseguire: ./dining_phil
 * cosa fa: i filosofi a cena tramite processi.
 * come funziona: il processo padre pulisce il filesystem di
 * un'eventuale presenza di semafori lasciati da precenti esecuzioni
 * non terminate correttamente e genera i processi figli (filosofi).
 * Se il segnale SIGUSR1 viene mandato a un filgio questo viene rigirato
 * al padre. Alla ricezione di un segnale di SIGUSR1 manda un segnale di
 * terminazione a tutti i figli, aspetta che siano tutti terminati e
 * ripulisce il sistema dalla presenza dei semafori.
 * La soluzione al problema dei filosofi a cena utilizzata e quella
 * descritta nel "piccolo libro dei semafori", NON quella descritta
 * nel Tanebaum che puo' causare starvation.
 * funzioni di rilievo da vedere nel man:
 * - processi (fork, wait, getpid)
 * - segnali(signal, kill)
 * - semafori (sem_open, sem_wait, sem_post, sem_close)
 * - varie (sleep, rand, srand).
 */

#include "dining_phil.h"

pid_t parent_pid;
pid_t sons[NPHIL];

// arriva il segnale di terminazione
void sig_user(int signum) {

    int i;

    printf("SIGUSR1 sent...\n");
    // se e' arrivato a un processo figlio, inoltralo al padre
    if (getpid()!=parent_pid)
        kill(parent_pid,SIGUSR1);
    // se sei il padre termina tutti i figli
    else
        for (i=0;i<NPHIL;i++)
            kill(sons[i],SIGTERM);
}

// la vita di un filosofo
int phil(int n) {
    sem_t *forks[5], *table;
    char philname[]="/phil_";
    int i, k;
    pid_t my_pid;

    // ottieni il pid
    my_pid=getpid();

    // apri i semaforo che indicano le posate (le risorse a
    // disposizione), se non esistono gia' li crei e imposti il valore
    // iniziale
    for (i=0;i<NPHIL;i++) {
        philname[5]=(char)('a'+i);
        if ((forks[i]=sem_open(philname,O_CREAT,S_IRUSR|S_IWUSR,1))==SEM_FAILED) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }
    }
    // apri il semaforo table (numero massimo di filosofi che possono
    // provare a mangiare assieme), se non esiste gia' lo crei e imposti
    // il valore iniziale
    if ((table=sem_open("/table",O_CREAT,S_IRUSR|S_IWUSR,NPHIL-1))==SEM_FAILED) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // imposta il seme iniziale del generatore psedocasuale
    srand(my_pid);

    // la dura vita del filosofo
    while (1) {
        // genera un intervallo casuale
        k=rand()%NSEC+1;
        printf("Philosopher %d - thinking for %d sec\n",n,k);
        // e pensa (cioe' dormi), se qualcuno ti sveglia e ancora non
        // hai finito continua a pensare finche' hai esaurito la materia
        // grigia
        while ((k=sleep(k)));

        // ora ti viene fame
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

    // chiudi i semafori aperti (il codice seguente non verra' mai
    // eseguito, comunque all'uscita il processo chiude in automatico
    // i semafori aperti)
    for (i=0;i<NPHIL;i++)
        sem_close(forks[i]);
    sem_close(table);
    printf("Process %d - clean done\n",my_pid);

    return 0;
}

int main(int argc, char *argv[]) {

    int nfork=NPHIL;
    int i;
    pid_t ok_fork;
    pid_t my_pid;
    int w;
    char philname[]="/phil_";

    // imposta il segnale di terminazione
    signal(SIGUSR1,sig_user);

    // chi sono io ?
    my_pid=getpid();
    parent_pid=my_pid;

    printf("Parent %d - number of philosphers: %d\n",my_pid,NPHIL);

    // cancella i file associati ai semafori possibilmente lasciati
    // dal un'esecuzione precedente non terminata correttamente
    for (i=0;i<NPHIL;i++) {
        philname[5]=(char)('a'+i);
        sem_unlink(philname);
    }
    sem_unlink("/table");

    // genera i figli
    for (i=0;i<nfork;i++) {
        if ((ok_fork=fork())<0) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }
        sons[i]=ok_fork;
        printf("Philosopher %d - pid %d\n",i,ok_fork);
        // caro papa' tuo figlio da grande vuole fare il filosofo !!!
        if (!ok_fork) {
            phil(i);
            exit(EXIT_SUCCESS);
        }
    }
    printf("Philosophers created\n");

    // attendi che tutti i figli abbiano concluso
    if (my_pid==getpid()) {
        for (i=0;i<nfork;) {
            wait(&w);
            if (WIFSIGNALED(w)) {
                i++;
            }
        }
        printf("Children exited\n");
    }

    // rimuovi i file associati ai semafori
    sem_unlink("/table");
    for (i=0;i<NPHIL;i++) {
        philname[5]=(char)('a'+i);
        sem_unlink(philname);
    }
    printf("Cleaning done\n");

    exit(EXIT_SUCCESS);
}
