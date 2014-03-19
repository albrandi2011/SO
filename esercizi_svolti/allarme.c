#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>

int n;

void sveglia(int num)
{
  //sveglio processo principale
}

void allarme(int num)
{
 int i;
 int pid[n];
 printf("Allarme lanciato\n");
  
 for (i=0;i<n;i++)
 { 
  if ((pid[i]=fork())<0)
  {
   printf("fork fallita...\n");
   exit(1);
  }
  else if(pid[i]==0)
  {
   printf("Sono il figlio %d il mio pid è %d\n",i,getpid());
  } 
  else
  {
   wait(NULL);
   printf("termina il figlio %d\n",i);
   exit(0);
  }
 }
} 


int main()
{
 int m,j=0;
 int principale;
 signal(SIGALRM,allarme);
 signal(SIGUSR1,sveglia);
 printf("Inserisci il numero n di processi da creare\n");
 scanf("%d",&n);

 printf("Inserisci il numero di secondi m\n");
 scanf("%d",&m);


 printf("Attendo %d secondi e lancio l'allarme\n",m);

 alarm(m);
 pause();
}
