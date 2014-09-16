#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
int *num,*pid,i,myi;
void sig_user1(int signum);
int main(int argc, char *argv[]);

void sig_user1(int signum)
{//getchar();// attendi invio
 int t=num[myi]+num[myi-1];
 printf("figlio %d pid %d tra %d mando SIGUSR1 a %d\n",i,getpid(),t,pid[i-1]);
           sleep(t);
 kill(pid[i-1], SIGUSR1);
 exit(i);
 }

int main(int argc, char *argv[])
{int t,w;
 num=malloc(argc*sizeof(int));
 pid=malloc(argc*sizeof(int));
 
 for(i=1;i<argc;i++)
  num[i-1]=atoi(argv[i]);
  
 // puntatori alle funzioni dei signal handler
 void (*sig_old1)(int);
 // sostituisci i signal handler
 sig_old1=signal(SIGUSR1,sig_user1); 
 for(i=0;i<argc-1;i++)
  {printf("num[%d]=%d\n",i,num[i]);
    pid[i]=fork();
    if(pid[i]==0)
      {myi=i;
       if(myi==argc-2) 
         {t=num[argc-2]+num[argc-3];
           printf("figlio %d pid %d tra %d mando SIGUSR1 a %d\n",i,getpid(),t,pid[i-1]);
           sleep(t);
           kill(pid[i-1], SIGUSR1);
          exit(i);
         }
        else
          while(1);
       
      } 
  
  }
  for (i=0;i<argc-1;i++) {
            // ottengo le informazioni sullo status di un figlio
            wait(&w);
        }
        printf("Really done\n");

 
// ripristina i signal handler originari
 signal(SIGUSR1,sig_old1);

 exit(EXIT_SUCCESS);
}

