#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>

typedef struct{
  pid_t pid;
  int num;
}str_processo;
void sig_print1(int signum){

}
int main(int argc, char *argv[]){
  int n=argc-1, i, t, j, w;
  pid_t padre;
  str_processo *processo;
   if(argc<2){
    printf("Errore.....\n");
   exit(EXIT_FAILURE);
     }
  
   processo=(str_processo *)malloc(n*sizeof(str_processo));
 
   for(i=0; i<n; i++){
    processo[i].num=atoi(argv[i+1]);
   }
  void (*sig_old1)(int);
   sig_old1=signal(SIGUSR1, sig_print1);
       padre=getpid();
       for(i=1; i<n; i++){
           processo[i].pid=fork();
            if(processo[i].pid==0){
               if(i==n-1){
                 for(j=i; j>0; j--){
                  t=processo[j].num+processo[j-1].num;
                   printf("Tra %d secondi mando il segnale e termino\n",t);
                   sleep(t);
                    kill(processo[j-1].pid,SIGUSR1);
                     }
                   }
            exit(i);
             }
         }

    if(padre){
      for(i=1; i<n;){
         wait(&w);
     if(WIFEXITED(w))
       i++;
      }
       }
   
  if(padre)
    printf("Ok finito!!\n");

  signal(SIGUSR1,sig_old1);
 exit(EXIT_SUCCESS);
}
