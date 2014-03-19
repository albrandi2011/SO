#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int id;
int *pid;
int t;
void pluto(int sig)
{
 printf("Ricevuto SIGUSR1 da %d!\n",id);
 printf("sono il figlio %d  aspetto %d secondi e mando SG\n",id,t);
 sleep(t);
 kill(pid[id-2], SIGUSR1);
           
exit(2);
}

int main(int argc, char *argv[])
 {if(argc<=3){printf("errore");exit(0);} 
  int status,i,p,n=argc-2; 

  pid=(int *) malloc(n*sizeof(int));
  for(i=1;i<=n;i++)
   { p= fork();
      pid[i-1]=p;
      if(p==0)//sono il figlio 
        {if(i==n) //sono l'ultimo                                       
           {t=atoi(argv[i])+atoi(argv[i+1]);
             printf("sono il figlio %d  aspetto %d secondi e mando SG\n",i,t);
             sleep(t);
             kill(pid[i-2], SIGUSR1);
            }
          else
           { signal(SIGUSR1, pluto);
              id=i; 
               t=atoi(argv[id])+atoi(argv[id+1]);
             
             while (1); // il figlio attende per sempre
           }
      } //if(p==0)
    }//for
  for(i=1;i<=n;i++) waitpid(pid[i],&status);
}


