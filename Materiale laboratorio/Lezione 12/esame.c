#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
typedef struct 
 {int id;
  pthread_t tid; 
  int k;
  int punteggio;
}  par_amici;
//semafori palle
sem_t palleverdi,pallerosse,pallenere;
//mutex campo
pthread_mutex_t campo;
//amiciRoutine
void *amiciRoutine(void *idp)
{par_amici *par;
  par=(par_amici *)idp;
  int i;
  int tipopalla=(rand()%(3)); 
  par->punteggio=0;
  printf("sono %d o scelto la palla %d\n",par->id,tipopalla);
  for(i=1;i<=par->k;i++)
   {printf("sono %d tiro  %d\n",par->id,i);
    switch(tipopalla)
     { case 0:sem_wait(&palleverdi); break;
        case 1:sem_wait(&pallerosse); break;
        case 2:sem_wait(&pallenere); break;
     }
    if (pthread_mutex_lock(&campo))
      printf("sono %d , errore pthread_mutex_lock \n",par->id);
    sleep( (rand()%2)+1);  
    par->punteggio+=(rand()%(11));  
    if (pthread_mutex_unlock(&campo))
      printf("sono %d , errore pthread_mutex_unlock \n",par->id);
    switch(tipopalla)
     { case 0:sem_post(&palleverdi); break;
        case 1:sem_post(&pallerosse); break;
        case 2:sem_post(&pallenere); break;
     }
   }
 return NULL;
}
// main
int main(int argc, char *argv[])
{par_amici *amici;
  int i, n, k;
  if (argc != 3)
   {printf("Use: esame #amici #tiri\n"); exit(-1); }
  n=atoi(argv[1]);
  if( (n<8) || (n>21))
   {printf("Use:  #amici errato\n"); exit(-1); }
  k=atoi(argv[2]);
 if( (k<10) || (k>30))
  {printf("Use:  #tiri errato\n"); exit(-1); }
 srand(time(NULL));
 sem_init(&palleverdi, 0, 2);
 sem_init(&pallerosse, 0, 2);
 sem_init(&pallenere, 0, 3);
 pthread_mutex_init(&campo, NULL);
 amici=(par_amici *)malloc(n*sizeof(par_amici)); 
 for (i=0; i<n; i++)
   {amici[i].id=i;amici[i].k=k;
     if (pthread_create(&amici[i].tid, NULL,amiciRoutine, &amici[i]))
       {printf(" Errore creazione thread\n"); exit(-1); }  
   }

 for (i=0; i<n; i++)
   if (pthread_join(amici[i].tid, NULL))
    {printf(" Errore pthread_join\n"); exit(-1); } 

 for (i=0; i<n; i++)
    printf(" punt[%d]=%d\n",i,amici[i].punteggio); 
 return 0;
}

