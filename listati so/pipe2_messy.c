/*
 * per compilare: gcc pipe2_messy.c -Wall -O3 -o pipe2_messy
 * per eseguire: ./pipe2_messy
 * cosa fa: generare una pipe, ereditata dai processi figli per
 * inviare dati al processo padre.
 * come funziona: il padre genera una pipe e successivamente i
 * figli la ereditano. Il padre legge dalla pipe mentre i figli
 * scrivono. Quando non ci sono piu' dati il padre termina.
 * Invece dello file stream (FILE *) vengono impiegati i descrittori a
 * file (int) nella pipe. I dati vengono generati in maniera random, e
 * il seme impostato in base alla data corrente e all'id univoco dei
 * figli per evitare che ad ogni esecuzione i dati forniti dai figli
 * siano identici. In teoria vista la concorrenza dei figli nello
 * scrivere nella pipe ci si aspetta che i dati si mescolino, cio' non
 * avviene a causa della bufferizzaione che fa si' che i dati vengano
 * scritti sulla pipe quando il buffer e' pieno o alla chiusura del
 * processo (nel caso di scrittura da terminale anche dopo aver pressato
 * invio). Per "OVVIARE" a tale problema viene forzata la scrittura
 * dentro la pipe (l'output varia da esecuzione ad esecuzione).
 * funzioni di rilievo da vedere nel man: 
 * - pipe (pipe)
 * - generatore pseudorandom (rand, rands)
 * - file stream/descrittore file (fdopen, fflush)
 * - processi (fork, getpid, getppid).
 */

#include "pipe2.h"

// funzione per leggere dalla pipe e mostrate l'output su schermo
void look_into_tube(int file) {
    FILE *fstream;
    int c;

    // converte da descrittore file a file stream per utilizzare fgetc
    fstream=fdopen(file,"r");   
    // finquando c'e' input dalla pipe mostralo sullo schermo
    // si blocca in attesa se la pipe e' vuota ma aperta, in caso
    // contrario da' EOF
    // notare che fgetc ritorna un intero per contenera EOF
    while ((c=fgetc(fstream))!=EOF)
        putchar(c);
    // chiude l'uscita della pipe
    fclose(fstream);
}

// funzione per scrivere nella pipe
void put_into_tube(int file) {
    FILE *fstream;
    int i;
    // seme
    time_t seed;

    // converte da descrittore file a file stream per utilizzare fgetc  
    fstream=fdopen(file,"w");
    // scrive qualcosa nella pipe
    fprintf(fstream,"writing\n");
    fprintf(fstream,"something:\n");
    // usa la data corrente piu' l'id univoco del prcesso come seme
    // altrimenti visto che i figli vengono  creati nell'arco dello
    // stesso secondo in genere, entrambi i figli scriverebbero gli
    // stessi dati.
    seed=time(NULL)+getpid();
    // imposta il seme
    srand(seed);
    // genera un carattere random e lo immette nella pipe
    for (i=0;i<26;i++) {
        // forza la scrittura nella pipe ora
        fflush(fstream);
        fputc('a'+(char)(rand()%26),fstream);
    }
    fprintf(fstream,"\n");      
    // chiude l'ingresso della pipe
    fclose(fstream);
}

int main(int argc, char *argv[]) {

    int i;
    int w;
    // pipe (tube[0] e' l'ingresso, in lettura, mentre tube[1]
    // l'uscita, in scrittura)
    int tube[2];
    pid_t ok_fork;
        
    // genera la pipe
    if (pipe(tube))  {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }
        
    // genera il primo figlio che eredita la pipe
    if ((ok_fork=fork())<0) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // se sono il padre genero il secondo figlio
    if (ok_fork)
        if ((ok_fork=fork())<0) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }   

    // c'e' bisogno che commento ?!?
    printf("Pid: %d\n",getpid());   
        
    if (!ok_fork) {
        // il figlio chiude il canale il lettura (deve solo scrivere)
        close(tube[0]);
        // e scrive nella pipe
        put_into_tube(tube[1]);
    } else {
        // il padre chiude il canale in scrittura (deve solo leggere)
        close (tube[1]);
        // e legge dalla pipe
        look_into_tube(tube[0]);
        // qui rilascia le risorse sul vettore di status del figlio
        i=0;
        while (i!=2) {
            wait(&w);
            if (WIFEXITED(w))
                i++;    
        } 
        // e attende in input da tastiera
        getchar();
    }   
        
    exit(EXIT_SUCCESS); 
}
