#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
typedef struct THDATASTR 
 {int punteggio;
   int     id; 
   pthread_t tid;
 } THDATA;

/* Define globally accessible variables and a mutex */

   THDATA thdat[NUMGIO]; 
   pthread_mutex_t campo;
   sem_t verde,rosso,nero;

void *giocatore(void *arg)
{THDATA *miodato;
  miodato=arg;
   int i,palla=rand()%3; 
  printf("Sono il giocatore %d e ho scelto la palla di tipo %d\n",miodato->id,palla);
  for(i=0;i<NUMTIRI;i++)
    {switch(palla)
       {case 0: sem_wait(&verde); break;
         case 1: sem_wait(&rosso); break;
         case 2:sem_wait(&nero); break;
       }
    printf("Sono il giocatore %d e ho preso  la palla %d\n",miodato->id,palla);  
    pthread_mutex_lock (&campo);
    printf("Sono il giocatore %d sto per tirare\n",miodato->id,palla);  
    miodato->punteggio+=rand()%11;
    
    printf("Sono il giocatore %d fino al tiro %d  ho fatto %d \n",miodato->id,i,miodato->punteggio);  
    
    pthread_mutex_unlock (&campo);
sleep(1);
switch(palla)
       {case 0: sem_post(&verde); break;
         case 1: sem_post(&rosso); break;
         case 2:sem_post(&nero); break;
       }
 }
   pthread_exit((void*) 0);
}

int main (int argc, char *argv[])
{
long i, NUMGIO, NUMTIRI;
void *status;
pthread_attr_t attr;
srand(time(NULL));
 
  NUMGIO=argv[1];
  NUMTIRI=argv[2];
for(i=0;i<NUMGIO;i++)
 {thdat[i].punteggio=0; thdat[i].id=i; }

pthread_mutex_init(&campo, NULL);
sem_init(&verde, 0, 2);sem_init(&rosso, 0, 2);sem_init(&nero, 0, 3);         

pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

for(i=0;i<NUMGIO;i++)
  {pthread_create(&(thdat[i].tid), &attr, giocatore, (void *)(&thdat[i])); }

pthread_attr_destroy(&attr);
/* Wait on the other threads */

for(i=0;i<NUMGIO;i++) {
  pthread_join(thdat[i].tid, &status);
  }
/* After joining, print out the results and cleanup */
for(i=0;i<NUMGIO;i++) {
printf ("punteggio[%d]= %d\n",(int)i, thdat[i].punteggio);
  }

sem_destroy (&verde);sem_destroy (&rosso);sem_destroy (&nero);

pthread_mutex_destroy(&campo);
pthread_exit(NULL);
}

  
  



