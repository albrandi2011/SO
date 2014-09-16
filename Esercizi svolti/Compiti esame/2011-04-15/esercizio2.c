#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>

void sign_1(int signum){
   printf("figlio, mi è arrivato SIGUSR1\n");
}
void sign_2(int signum){
   printf("padre, mi è arrivato SIGUSR2\n");

}

int main(int argc, char *argv[]){
 int N, i, w;
  pid_t pid, padre;
     void (*sig_old1)(int);
   void (*sig_old2)(int);

 if(argc!=2){
   printf("Errore nell'inserimento dei parametri\n");
    exit(-1);
   }
  N=atoi(argv[1]);
  
     sig_old1=signal(SIGUSR1,sign_1);
     sig_old2=signal(SIGUSR2,sign_2);
    if(N<1){
      printf("Hai inserito un valore < 1\n");
     exit(-1);
        }
     padre=getpid();
     for(i=1; i<=N; i++){
        pid=fork();
         if(pid==0){
            printf("Sono il figlio di livello %d\n",i);
            if((i>0)&&(i<N))
               kill(padre,SIGUSR1);
             if(i==N)
               pause();
              }
         if(pid>0){
            kill(padre,SIGUSR2);
           wait(&w);
           }
       }
   signal(SIGUSR1,sig_old1);
   signal(SIGUSR2,sig_old2);
exit(EXIT_SUCCESS);
}
