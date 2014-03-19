#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char *argv[])
{pid_t ok_fork;
 pid_t my_pid;
 int w, nfork, i;
  
   nfork=argv[1];
  if(argc!=2)
    if (!sscanf(argv[2],"%d",&nfork)) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }

 
   printf("Number of forks: %d\n",nfork);
    // pid del padre
    my_pid=getpid();

    // genero i figli richiesti
    for (i=0;i<nfork;i++) {
        if ((ok_fork=fork())<0) {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }

        // se sono il figlio mostro le informazioni relative sullo
        // schermo e termino (senza il break finale anche i figli a loro
        // volta iniziano a generare figli !!!)
        if (!ok_fork) {
            printf("I'm the %d-th child\n",i);
            break;
        }
    }

    // se sono il padre attendo che i figli terminino
    if (my_pid==getpid()) {
        printf("Done\n");
        // per un numero pari al numero dei figli
        for (i=0;i<nfork;) {
            // ottengo le informazioni sullo status di un figlio
            wait(&w);
            // se ha terminato attendo i figli rimasti
            if (WIFEXITED(w))
                i++;
        }
        printf("Really done\n");
    }

    exit(EXIT_SUCCESS);
}
 
