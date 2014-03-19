#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include<malloc.h>
#include<semaphore.h>
#include<time.h>


int tmp;
int N;
int *tempi;
pthread_t *nuotatori;
sem_t corsie;

struct amico
{
 int id;
 int num_corsia;
};

void *corsa(void *attr)
{
  
  struct amico *usr1;
  usr1=(struct amico *)attr;
  int id=usr1->id;
  int num_corsia=usr1->num_corsia;
  
  printf("Il nuotatore %d scende nella corsia %d e comincia la gara\n",usr1->id,usr1->num_corsia);
  tmp=(rand()%40+160)*1000;
  tempi[usr1->id]=tempi[usr1->id]+(tmp/1000);
  usleep(tmp);
  
  printf("Il nuotatore %d libera la corsia tempo=%d\n",usr1->id,tempi[usr1->id]); 
  sem_post(&corsie); 
}

int main()
{
  int i,j;
  int valore;
  srand(time(NULL));
  printf("Inserisci il numero N di partecipanti al torneo di nuoto\n");
  scanf("%d",&N);
  
  nuotatori=malloc(sizeof(pthread_t)&N);
  tempi=(int *)malloc(N*sizeof(int));

  sem_init(&corsie,0,8);

  for (i=0;i<N;i++)
   tempi[i]=0;

  for (i=0;i<4;i++)
  {
   if (i==0)
    {     
     printf("Stile dorso\n");
     for(j=0;j<N;j++)
     {
      sem_wait(&corsie);
      sem_getvalue(&corsie,&valore);
      struct amico usr;
      usr.id=j;
      usr.num_corsia=valore;
      pthread_create(&nuotatori[j],NULL,corsa,(void *)&usr);
      usleep(200000);
     }
    pthread_join(nuotatori[N-1],NULL);
    }
   else if(i==1)
    {
     //libero
      printf("Stile libero\n");
     for(j=0;j<N;j++)
     {
      sem_wait(&corsie);
      sem_getvalue(&corsie,&valore);
      struct amico usr;
      usr.id=j;
      usr.num_corsia=valore;
      pthread_create(&nuotatori[j],NULL,corsa,(void *)&usr);
      usleep(200000);
     }
    pthread_join(nuotatori[N-1],NULL);
    }
   else if(i==2)
    {
     //rana
     printf("Stile rana\n");
     for(j=0;j<N;j++)
     {
      sem_wait(&corsie);
      sem_getvalue(&corsie,&valore);
      struct amico usr;
      usr.id=j;
      usr.num_corsia=valore;
      pthread_create(&nuotatori[j],NULL,corsa,(void *)&usr);
      usleep(200000);
     }
    pthread_join(nuotatori[N-1],NULL);
    }
   else
    {
     //delfino
     printf("Stile delfino\n");
     for(j=0;j<N;j++)
     {
      sem_wait(&corsie);
      sem_getvalue(&corsie,&valore);
      struct amico usr;
      usr.id=j;
      usr.num_corsia=valore;
      pthread_create(&nuotatori[j],NULL,corsa,(void *)&usr);
      usleep(200000);
     }
     pthread_join(nuotatori[N-1],NULL);
    }
   sleep(1);
  } 

int b[N],aux1,aux2,scambia=0;

for(i=0;i<N;i++)
  b[i]=i;

//bubblesort
 while(scambia==0)
  {
   for (j=0;j<N-1;j++)
   {
    if (tempi[j]>tempi[j+1])
    {
      aux1=tempi[j]; aux2=b[j];
      tempi[j]=tempi[j+1]; b[j]=b[j+1];
      tempi[j+1]=aux1; b[j+1]=aux2;
      scambia=0;
    }
    else
    scambia=1;
   }
  }

 printf("Stampa della classifica\n");

 for(i=0;i<N;i++)
   printf("%d) Nuotatore %d tempo impiegato: %dms\n",i,b[i],tempi[i]);

 printf("Il vincitore del torneo è il Nuotatore %d\n",b[0]);
}
