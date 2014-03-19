//Sergio Caruso M:0538499
//Il programma funziona nella seguente maniera:
//dopo aver calcolato il numero di gruppi e di conseguenza si sanno quante iterazioni si dovranno fare
//per ogni iterazione si creano tanti figli quanti sono i go-kart, se non sono l'ultimo figlio vado in pausa 
//perchè vi sono ancora kart disponibili, se sono l'ultimo figlio ovvero quello che prende l'ultimo go-kart 
//invio un segnale di sveglia al fratello, quando tutti i fratelli sono stati svegliati possono effettuare 
//il giro
//questo procedimnto viene utilizzato per ogni gruppo  

#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<time.h>
#include<malloc.h>

int t=0,NT,K;
int iter, G;
int i;

void sveglia(int num)
{
 if (i==0)
 {
  printf("Le persone effettuano il giro\n");
  sleep(1); 
 }
 else
 {
 
  kill(getpid()-1,SIGUSR2);
 }

}

void pista(int num)
{
 if (i==(K-1))
 { //possono partire
  printf("Sono il figlio %d i go-kart sono tutti pieni comunico agli altri che si può partire (PID: %d)\n",i,getpid());
  kill(getpid()-1,SIGUSR2);
 }
 else
 {
  printf("Sono il figlio %d attendo che si riempano tutti i go-kart (PID: %d)\n",i,getpid());
  pause();
 }
}

int main()
{

 int *pid;
 signal(SIGUSR1,pista);
 signal(SIGUSR2,sveglia);
 
 printf("Inserire il numero (NT) totale di utenti\n");
 scanf("%d",&NT);
 
 printf("Inserire il numero il numero (K) di componenti per gruppo (NT/K deve essere un intero)\n");
 scanf("%d",&K);
 
 iter=NT/K;
 G=K;
 
 printf("Ci saranno %d gruppi da %d persone\n",iter,K);
 
 pid=(int *)malloc(K*sizeof(int *));
 
 while(t<iter)
 {
  printf("Gruppo %d\n",t);
  
  for(i=0;i<G;i++)
  {
   sleep(1);
   if ((pid[i]=fork())<0)
   {
     printf("Errore Fork! \n");
     exit(1);
   }
   else if(pid[i]==0)
   { //figlio
     kill(getppid(),SIGUSR1);
   }
   else
   { //padre
     wait(NULL);
     exit(0);
   }
  }
  sleep(1);
  t++; 
 }
 exit(0);
}
