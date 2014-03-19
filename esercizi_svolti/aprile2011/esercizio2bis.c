#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>

//funzioni signal handler
void sig_usr_print1(int signum);
void sig_usr_print2(int signum);

void sig_usr_print1(int signum){
    printf("Ho ricevuto SIGUSR1...\n");

 exit(1);
}
void sig_usr_print2(int signum){
    printf("Ricevuto SIGUSR2...\n");
 exit(1);
}
int main(int argc, char *argv[]){
  int N, i, status;
  pid_t padre, figlio, figlioN;
  
   if(argc!=2){  //controllo se si inserisce l'intero
     printf("Si è verificato un'errore nell'inserimento dei parametri\n");
      exit(EXIT_FAILURE);
    }

   N=atoi(argv[1]); //intero inserito
   
     if(N<1){
        printf("L'intero deve essere >0!!\n");
      exit(EXIT_FAILURE);
    }
//definizioni puntatori signal handler
    void(*sig_old1)(int);
    void(*sig_old2)(int);

     sig_old1=signal(SIGUSR1,sig_usr_print1);
     sig_old2=signal(SIGUSR2,sig_usr_print2);
     padre=getpid();

     for(i=1; i<N+1; i++){
         figlio=fork();
        if(figlio==0){
          printf("Sono il figlio di livello %d con pid %d\n",i, getpid());
           if(i==N)
            kill(padre,SIGUSR2);
          if(i<N)
          kill(padre,SIGUSR1);
         }
        if(figlio>0){
          printf("Sono il processo padre di livello %d con pid %d\n",i, getpid());
            if(i<N)
            kill(padre,SIGUSR1);
            if(i==N)
            kill(padre,SIGUSR2);
           }
        }

    for(i=0; i<N;i++){
      wait(&status);
     }
   signal(SIGUSR1,sig_old1);
    signal(SIGUSR2,sig_old2);
 exit(EXIT_SUCCESS);
}
