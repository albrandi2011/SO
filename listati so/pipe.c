/*
 * per compilare: gcc pipe.c -Wall -O3 -o pipe
 * per eseguire: ./pipe
 * cosa fa: generare una pipe, utilizzata dal processo figlio
 * per inviare dati al processo padre.
 * come funziona: il padre genera una pipe e successivamente un
 * figlio che la eredita. Il padre legge dalla pipe mentre il figlio
 * scrive. Quando non ci sono piu' dati il padre termina. 
 * Invece dello file stream (FILE *) vengono impiegati i descrittori a
 * file (int) nella pipe. I dati vengono generati in maniera random, e
 * il seme impostato in base alla data corrente per evitare che ad
 * ogni esecuzione i dati forniti dal figlio siano identici.
 * funzioni di rilievo da vedere nel man: 
 * - pipe (pipe)
 * - generatore pseudorandom (rand, rands)
 * - file stream/descrittore file (fdopen)
 * - processi (fork, getpid, getppid).
 */
 
#include "bad_pipe.h"

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
    // usa la data corrente come seme
    seed=time(NULL);
    // imposta il seme
    srand(seed);
    // genera un carattere random e lo immette nella pipe
    for (i=0;i<26;i++)
        fputc('a'+(char)(rand()%26),fstream);
    fprintf(fstream,"\n");      
    // chiude l'ingresso della pipe
    fclose(fstream);
}

int main(int argc, char *argv[]) {

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
        
    // genera il figlio che eredita la pipe
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
        while (1) {
            wait(&w);
            if (WIFEXITED(w))
                break;  
        } 
        // e attende in input da tastiera
        getchar();
    }   
        
    exit(EXIT_SUCCESS); 
}
