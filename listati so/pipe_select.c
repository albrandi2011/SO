/*
 * per compilare: gcc pipe_select.c -Wall -O3 -o pipe_select
 * per eseguire: ./pipe_select
 * cosa fa: generare due pipe, riempite ad intervalli casuali da
 * processi figli, controllarle e mostrare il loro contenuto in caso
 * e' stato scritto qualcosa, oppure passato un determianto arco di
 * tempo avvisare.
 * come funziona: il padre genera due pipe ereditate da due figli che
 * scriveranno in esse ad intervalli casuali (il resto del termpo
 * resteranno sospesi), se arriva un segnale di tipo SIGUSR1
 * termineranno, nel caso di SIGUSR2 mostreranno un avviso e torneranno
 * in sospensione fino allo scadere del tempo. Il padre controlla la
 * pipe con select, restando sospeso fino all'arrivo di un segnale o
 * finche' una pipe non e' pronta oppure fino allo scadere di un timer
 * (piu' preciso di quello di alarm). Quando entrambe le pipe sono
 * chiuse dai figli termina
 * funzioni di rilievo da vedere nel man:
 * - pipe (pipe)
 * - segnali (signal)
 * - varie (times, errno)
 * - generatore pseudorandom (rand, rands)
 * - file stream/descrittore file (fdopen, fflush, select, FDZERO,
 *   FDSET, FDISSET, FD_SETSIZE, fflush, fclose)
 * - processi (fork, getpid, getppid).
 */

#include "pipe_select.h"

volatile sig_atomic_t go_ahead=1;

// se arriva SIGUSR1 preparati a terminare il processo
void sig_cont(int signum) {
    go_ahead=0;
}

// segnala l'arrivo di SIGUSR2
void sig_dummy(int signum) {
    printf("A signal arrived\n");
}

// leggi dalle pipe
void look_into_tube(int *file) {
    FILE *fstream[2];
    int c, i;
    fd_set set;
    int ms;
    int nopen;
    int who_open[2]={1,1};
    struct timeval stimer;

    // ottieni ii descrittori file per ciascuna pipe
    fstream[0]=fdopen(file[0],"r");
    fstream[1]=fdopen(file[2],"r");
    // sono entrambe attive
    nopen=2;

    // finche le pipe sono aperte
    while (nopen) {
        // finche' nessuna pipe e' pronta
        do {
            // imposta come vuoto l'insieme dei descrittori file
            FD_ZERO(&set);
            // se la prima pipe e' aperta
            if (who_open[0])
                // aggiungi il descrittore relativo della pipe
                // all'insieme
                FD_SET(file[0],&set);
            // se la second pipe e' aperta
            if (who_open[1])
                // aggiungi il descrittore relativo della pipe
                // all'insieme
                FD_SET(file[2],&set);

            // imposta il timer per il controllo delle pipe
            stimer.tv_sec=5;
            stimer.tv_usec=0;

            // attendi finche il timer non scade oppure una pipe e'
            // pronta
            ms=select(FD_SETSIZE,&set,NULL,NULL,&stimer);

            // gestisci gli errori
            if (ms==-1) {
                switch (errno) {
                    case EBADF:
                        printf("Error EBADF\n");
                    break;
                    case EINTR:
                        printf("Error EINTR\n");
                    break;
                    case EINVAL:
                        printf("Error EINVAL\n");
                    break;
                    default:
                        printf("Error %d\n",errno);
                }
            }

            // se il timer e' scaduto avvisa che nessuno ha scritto
            if (!ms) {
                printf("Nobody writes\n");
            }
        }   while (!ms);

        // visualizza il numero di pipe pronte
        printf("Ready pipes: %d\n",ms);

        // se e' pronta la prima pipe
        if (FD_ISSET(file[0],&set)) {
            // leggi in contenuto e mostralo
            printf("1st said:\n");
            for(i=0;(i<26)&&((c=fgetc(fstream[0]))!=EOF);i++)
                putchar(c);
            // se la prima pipe e' stata chiusa mostra un avviso
            if (c==EOF) {
                printf("1st has gone\n");
                // e ricordatelo
                who_open[0]=0;
                // oltre che chiudi il descrittore relativo
                fclose(fstream[0]);
            } else
            printf("\n");
        }
        // se e' pronta la seconda pipe
        if (FD_ISSET(file[2],&set)) {
            // leggi in contenuto e mostralo
            printf("2nd said:\n");
            for(i=0;(i<26)&&((c=fgetc(fstream[1]))!=EOF);i++)
                putchar(c);
            // se la prima pipe e' stata chiusa mostra un avviso
            if (c==EOF) {
                printf("2nd has gone\n");
                // e ricordatelo
                who_open[1]=0;
                // oltre che chiudi il descrittore relativo
                fclose(fstream[1]);
            } else
            printf("\n");
        }
        // aggiorna il numero di pipe aperto
        nopen=who_open[0]+who_open[1];
    }
    // finito
    printf("No more data\n");
}

void put_into_tube(int file) {
    FILE *fstream;
    int i;
    int t, old_t;

    // imposta il signal handler per la terminazione del processo
    // tramite SIGUSR1
    signal(SIGUSR1,sig_cont);
    signal(SIGUSR2,sig_dummy);
    fstream=fdopen(file,"w");

    // finche' non e' stato mandato il segnale SIGUSR1
    while (go_ahead) {
        // genera un intervallo random di sospensione del processo
        old_t=rand()%15+1;
        printf("Pid %d - Timer after %d sec\n",getpid(),old_t);

        // resta sospeso finche' non arriva il segnale SIGUSR1 o per il
        // tempo fissato (nel caso di interruzione da altri segnali
        // risospendi per il tempo restante)
        while (go_ahead&&(t=sleep(old_t))) {
            // mostra quanto tempo manca in caso di arrivo di un segnale
            printf("Pid: %d - Timer breaks and %d sec remain\n",getpid(),t);
            old_t=t;
        }

        // se e' arrivato SIGUSR1 termina
        if (!go_ahead)
            break;

        // stampa qualcosa
        printf("Pid: %d - Writing\n",getpid());

        // di causuale
        for (i=0;i<26;i++)
            putc('a'+(char)(rand()%26),fstream);
        // e aggiorna il contenuto nella pipe
        fflush(fstream);
    }
    printf("Pid %d - A child died :'(\n",getpid());
    // chiudi la pipe
    fclose(fstream);
}

int main(int argc, char *argv[]) {

    int w;
    int i;
    int tube[4];
    pid_t ok_fork[2]={-1,-1};

    tictac t;

    // crea una prima pipe
    if (pipe(tube))  {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // crea una seconda pipe
    if (pipe(&tube[2]))  {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // genera un primo figlio
    if ((ok_fork[0]=fork())<0) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // genera un secondo figlio
    if (!ok_fork[0]) {
        t.t=times(&t.c);
        srand((int)t.t%9741);
    }

    // esci in caso di errori
    if ((ok_fork[0])&&((ok_fork[1]=fork())<0)) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // se sono il padre
    if ((ok_fork[0]>0)&&(ok_fork[1]>0)) {
        printf("Parent pid: %d\n"\
               "1st child pid: %d\n"\
               "2nd child pid: %d\n",getpid(),ok_fork[0],ok_fork[1]);
        // chiudi le pipe in scrittura
        close(tube[1]);
        close(tube[3]);
        // leggi dalle pipe
        look_into_tube(tube);

        i=0;
        // attendi che entrambi i figli terminino
        while (i!=2) {
            wait(&w);
            if (WIFEXITED(w))
                i++;
        }

        printf("Done\n");
    }

    // se sono il primo figlio
    if (!ok_fork[0]) {
        // chiudi i descrittori di pipe non necessari
        close(tube[0]);
        close(tube[2]);
        close(tube[3]);
        // scrivi nella pipe
        put_into_tube(tube[1]);
    }

    if (!ok_fork[1]) {
        // chiudi i descrittori di pipe non necessari
        close(tube[0]);
        close(tube[1]);
        close(tube[2]);
        // scrivi nella pipe
        put_into_tube(tube[3]);
    }

    exit(EXIT_SUCCESS);
}
