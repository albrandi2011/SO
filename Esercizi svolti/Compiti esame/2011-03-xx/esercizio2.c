#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>

void sign_print1(int signum){
   printf("Sono NONNO: Un nipote mi ha inviato SIGUSR1\n");
}
void sign_print2(int signum){
  printf("Sono NONNO: Un nipote mi ha inviato SIGUSR2\n");
}
int main(int argc, int *argv[]){
 int i, t, w, k;
 pid_t padre, figlio, nipote;
   
   if(argc!=2){
    printf("numero dei parametri nn corretti\n");
   exit(-1);
  }
 t=atoi(argv[1]);
//puntatori handle
 void(*sig_old1)(int);
 void(*sig_old2)(int);
   
   sig_old1=signal(SIGUSR1, sign_print1);
   sig_old2=signal(SIGUSR2, sign_print2);

  padre=getpid();
    printf("Sono il padre, il mio pid e' %d\n",padre);
   for(i=0; i<2; i++){
     figlio=fork();
      if(figlio==0){
         sleep(t);
      printf("Sono il figlio, il mio pid e' %d\n",getpid());
       nipote=fork();
      if(nipote==0){
         sleep(t);
       printf("Sono il nipote, il mio pid e' %d\n",getpid());
         if(i==0)
          kill(padre,SIGUSR1);
         if(i==1)
           kill(padre,SIGUSR2);
         }
        wait(&w);
      exit(i);
    }
  wait(&k);
  }
  signal(SIGUSR1,sig_old1);
  signal(SIGUSR2,sig_old2);
exit(EXIT_SUCCESS);
}
