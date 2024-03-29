//funge
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>

typedef struct{ //struttura che definisce il natante
  pthread_t tid;
  int id;
  int peso;
}str_natante;

sem_t canoa;
pthread_mutex_t pedalo;
int T, somma=0;
void *routine_affitto(void *arg){
  str_natante *natante;
  natante=arg;
   srand(time(NULL));
     switch(rand()%2){
     case 0: printf("Sono il natante %d, ho scelto la canoa, provo a prenderne una\n", natante->id);
                sem_wait(&canoa);
              printf("Sono il natante %d, sto facendo il giro in canoa! che divertimento!!\n",natante->id);
               sleep(T);
              sem_post(&canoa);
              printf("Sono il natante %d, è finito il giro che tristezza!!\n",natante->id);
                      break;
     case 1: printf("Sono il natante %d, ho scelto di fare un giro in pedalò\n",natante->id);
               somma=somma+natante->peso;
                if(somma>=500){
                  pthread_mutex_lock(&pedalo);
                 printf("Stiamo facendo il giro sul pedalò, evviva!!\n");
                  sleep(T);
                  pthread_mutex_unlock(&pedalo);
                 printf("Il giro è finito...che peccato!!\n");
                  somma=0;
                      }
                break;
      }
pthread_exit((void *)0);
}
int main(int argc, char *argv[]){
 int N, i, rc;
 str_natante *natante;
 pthread_attr_t attr;
 void *status;

  if(argc!=3){  //se non si inseriscono i due parametri da errore 
    printf("Errore!! mancano parametri\n");
   exit(-1);
   }
 N=atoi(argv[1]);  //numero amici inseriti in input
 T=atoi(argv[2]);  //tempo inserito in input

  if(N<9){  // il numero di amici deve essere >= 9 altrimenti darà errore
    printf("Non consentito!!\n");
  exit(-1);
   }
 
  natante=(str_natante *)malloc(N*sizeof(str_natante));  //allocazione dinamica
   //inizializzazione semaforo
      sem_init(&canoa,0,2);
   //inizializzazione mutex
     pthread_mutex_init(&pedalo, NULL);
   //inizializzazione thread
      pthread_attr_init(&attr);
       pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   srand(time(NULL));
    for(i=0; i<N; i++){
       natante[i].id=i;
       natante[i].peso=(rand()%91)+51;  //scelgo il peso in modo random per ogni amico
      rc=pthread_create(&natante[i].tid,&attr,routine_affitto,(void *)(&natante[i]));
        if(rc){
          printf("Errore!!");
        exit(-1);
       }
    }
   //rilascio delle risorse da parte dei thread
     pthread_attr_destroy(&attr);
      for(i=0; i<N; i++){
       pthread_join(natante[i].tid,&status);
      }
  //rilascio risorse da parte dei mutex
     pthread_mutex_destroy(&pedalo);
 //rilascio risorse da parte del semaforo
    sem_destroy(&canoa);
 pthread_exit(NULL);
}
    
