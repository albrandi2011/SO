#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#define N 5
#define K 10

sem_t palle_v,palle_r,palle_n;
pthread_t giocatori[N];
pthread_mutex_t campo=PTHREAD_MUTEX_INITIALIZER;

int punteggi[N];

struct utente
{
  int id;
  int colore;
};

void *tiro(void *usr)
{
  struct utente *tiratore;
  tiratore=(struct utente *)usr;
  int id=tiratore->id;
  int colore=tiratore->colore;

  pthread_mutex_trylock(&campo);

  punteggi[tiratore->id]=punteggi[tiratore->id]+ rand()%10 +1;
  if (tiratore->colore==0)
  {
    printf("il giocatore %d ha effettuato il tiro con la palla verde\n",tiratore->id);
    sleep(rand()%8 +1);
    sem_post(&palle_v);
  }
  else if(tiratore->colore==1)
  {
    printf("il giocatore %d ha effettuato il tiro con la palla rossa\n",tiratore->id);
    sleep(rand()%8 +1);
    sem_post(&palle_r);
  }
  else
  {
    printf("il giocatore %d ha effettuato il tiro con la palla nera\n",tiratore->id);
    sleep(rand()%8 +1);
    sem_post(&palle_n);
  }



  pthread_mutex_unlock(&campo);
}

int main()
{
  int lanci=1,i;
  int palla,rc;

sem_init (&palle_v,0,2);
sem_init (&palle_r,0,2);
sem_init (&palle_n,0,3);

for(i=0;i<N;i++)
{
 punteggi[i]=0;
}

  while(lanci<=K)
  {
      printf("Serie di lanci n° %d\n",lanci);
      for (i=0;i<N;i++)
      {
        srand(time(NULL));
        palla=rand()%3 ;

        if (palla==0)
        {
          printf("Il giocatore %d sceglie la palla verde\n",i);
          sem_wait(&palle_v);
          printf("Il giocatore %d acquisice la palla verde\n",i);
          struct utente usr;
          usr.id=i;
          usr.colore=0;
          sleep(1);
          rc=pthread_create(&giocatori[i],NULL,tiro,(void *)&usr);
          if (rc)
          {
            printf("ERROR...");
            exit(1);
          }
        }
        else if(palla==1)
        {
          printf("Il giocatore %d sceglie la palla rossa\n",i);
          sem_wait(&palle_r);
          printf("Il giocatore %d acquisice la palla rossa\n",i);
          struct utente usr;
          usr.id=i;
          usr.colore=1;
          sleep(1);
          rc=pthread_create(&giocatori[i],NULL,tiro,(void *)&usr);
          if (rc)
          {
            printf("ERROR...");
            exit(1);
          }
        }
        else
        {
          printf("Il giocatore %d sceglie la palla nera\n",i);
          sem_wait(&palle_n);
          printf("Il giocatore %d acquisice la palla nera\n",i);
          struct utente usr;
          usr.id=i;
          usr.colore=2;
          sleep(1);
          rc=pthread_create(&giocatori[i],NULL,tiro,(void *)&usr);
          if (rc)
          {
            printf("ERROR...");
            exit(1);
          }
        }
      }
    pthread_join(giocatori[N-1],NULL);
    lanci++;
  }


printf("Classifica Finale\n");
for(i=0;i<N;i++)
{
  printf("Punteggio giocatore %d = %d\n",i,punteggi[i]);
}

  pthread_exit(NULL);
}
