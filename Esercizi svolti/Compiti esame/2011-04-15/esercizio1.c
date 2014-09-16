#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>

sem_t stanza;
typedef struct{
   pthread_t tid;
    int id;
}str_turista;
void *routine_gioconda(void *arg){
 str_turista *turista;
  turista=arg;
      srand(time(NULL));

         sleep(rand()%10); //attendono un tempo casuale
      printf("Sono il turista %d, cerco di entrare nella stanza della gioconda\n",turista->id);
        sem_wait(&stanza);
      printf("Sono il turista %d, sto ammirando la gioconda...WOW!!!\n",turista->id);
          sleep(rand()%10); //tempo casuale x ammirare il quadro
       sem_post(&stanza);
pthread_exit((void *)0);
}
int main(int argc, char *argv[]){
  int N, i, rc;
  void *status;
  pthread_attr_t attr;
  str_turista *turista;

   if(argc!=2){
      printf("Numeri di parametri errato\n");
    exit(EXIT_FAILURE);
     }

  N=atoi(argv[1]);  //numero di turisti
   turista=(str_turista *)malloc(N*sizeof(str_turista));  //allocazione dinamica
    
  //inizializzazione semaforo, serve per rappresentare la stanza
     sem_init(&stanza,0,5);
  //inizializzazione thread
      pthread_attr_init(&attr);  //settato a joinable
       pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
    for(i=0; i<N; i++){
      turista[i].id=i;
     rc=pthread_create(&turista[i].tid,&attr,routine_gioconda,(void *)(&turista[i]));
      if(rc){
         printf("Errore!!!");
      exit(-1);}
     }
   
   //rilascio delle risorse per i thread 
        pthread_attr_destroy(&attr);
     for(i=0; i<N; i++)
       pthread_join(turista[i].tid,&status); //attendo la terminazioni di tutti i thread

   //rilascio risorse per i thread
      sem_destroy(&stanza);

 pthread_exit(NULL);    
}
