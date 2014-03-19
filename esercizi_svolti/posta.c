#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>
#include<malloc.h>
#include<time.h>


int M,N;
int turno=0;

int postazioni[3]={0,0,0};
int contatori[4]={0,0,0,0};
sem_t pagamenti[3];
sem_t spedizione;
sem_t sedia;

pthread_t *utente;

struct cliente
{
   int numero;
   int tipo;
};

void *sportelli(void *op)
{
   struct cliente *usr1;
   usr1=(struct cliente *)op;
   int numero=usr1->numero;
   int tipo=usr1->tipo;

   if (usr1->tipo==0)
   {
        if (postazioni[0]==0)
        {
          sem_wait(&pagamenti[0]);
          sem_post(&sedia);
          postazioni[0]=1;
          printf("Il cliente n° %d va allo sportello 1 \n",usr1->numero);
          sleep(rand()% 10+1);
          printf("operazione effettuata allo sportello 1\n");
          contatori[0]++;
          sem_post(&pagamenti[0]);
          postazioni[0]=0;
        }
        else if (postazioni[1]==0)
        {
          sem_wait(&pagamenti[1]);
          sem_post(&sedia);
          postazioni[1]==1;
          printf("Il cliente n° %d va allo sportello 2 \n",usr1->numero);
          sleep(rand()% 10+1);
          printf("operazione effettuata allo sportello 2\n");
          contatori[1]++;
          sem_post(&pagamenti[1]);
          postazioni[1]=0;
        }
        else
        {
          sem_wait(&pagamenti[2]);
          sem_post(&sedia);
          postazioni[2]==1;
          printf("Il cliente n° %d va allo sportello 3 \n",usr1->numero);
          sleep(rand()% 10+1);
          printf("operazione effettuata allo sportello 3\n");
          contatori[2]++;
          sem_post(&pagamenti[2]);
          postazioni[2]=1;
        }
   }
  else
  {
    sem_wait(&spedizione);
    sem_post(&sedia);

    printf("Il cliente n° %d va allo sportello 4 \n",usr1->numero);
    sleep(rand()% 10+1);
    printf("operazione effettuata allo sportello 4\n");
    contatori[3]++;
    sem_post(&spedizione);
  }
}

int main()
{
    int tipologia,i;

    printf("Inserisci il turno massimo da raggiungere\n");
    scanf("%d",&M);
    printf("Inserisci il numero di sedie\n");
    scanf("%d",&N);

    sem_init (&sedia,1,N);
    sem_init (&pagamenti[0],0,1);
    sem_init (&pagamenti[1],0,1);
    sem_init (&pagamenti[2],0,1);
    sem_init (&spedizione,0,1);

    utente=malloc(sizeof(pthread_t)*M);

    while (turno<M)
    {
        srand(time(NULL));
        struct cliente usr;
        usr.numero=turno;
        usr.tipo=rand()% 2;

        sem_wait(&sedia);

        if (usr.tipo==0)
        printf("L'utente prende il numero %d deve pagare e si siede\n",usr.numero);
        else
        printf("L'utente prende il nemero %d deve spedire e si siede\n",usr.numero);

        sleep(1);
        pthread_create(&utente[turno],NULL,sportelli,(void *)&usr);
        turno++;

        sleep(1);
    }

   pthread_join(utente[M-1],NULL);

    sleep(2);
   for (i=0;i<4;i++)
    printf("Numero di persone servite allo sportello %d: %d\n",i,contatori[i]);

   pthread_exit(NULL);

}
