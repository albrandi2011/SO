#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>

pthread_mutex_t bersaglio;
sem_t archi, freccie;
int A, I, K;
 typedef struct{
  pthread_t tid;
  int id;
  int parziale;
  int totale;
}str_tiratore;

void *routine_tiro(void *arg){
 str_tiratore *tiratore;
  int x, i, j;
  tiratore=arg;
    for(i=0; i<K; i++){
    printf("Sono il tiratore %d, cerco di prendere un arco e una freccia\n",tiratore->id);
      sem_wait(&archi); //prendo un arco
       sem_wait(&freccie); //prendo una freccia
      pthread_mutex_lock(&bersaglio); //occupo il bersaglio x il tiro
     tiratore->parziale=(rand()%10)+1;  //punteggio casuale
    printf("Sono il tiratore %d, ho fatto %d punti\n",tiratore->id, tiratore->parziale);
     sem_post(&archi); //rilascio l'arco
      sleep(5);
      sem_getvalue(&freccie,&x); //vedo quante freccie sono state usate
    
     if(x==0){ //se sono state usate tutte
         printf("Sono finite le freccie\n");
       tiratore->totale+=tiratore->parziale; //aggiorno il punteggio
       for(j=0; j<I; j++)
       sem_post(&freccie);} //riporto indietro le freccie
      pthread_mutex_unlock(&bersaglio);
  }
  pthread_exit((void *)0); 
}

int main(int argc, char *argv[]){
 int N, i, rc;
 str_tiratore *tiratore;
 void *status;
 pthread_attr_t attr;
  
   if(argc!=5){
    printf("Numero dei parametri inseriti errato....\n");
   exit(EXIT_FAILURE);
    }
 N=atoi(argv[1]); //numero appassionati di tiro con l'arco
 K=atoi(argv[2]); //numero dei tiri a disposizione
 A=atoi(argv[3]); //numero di archi
 I=atoi(argv[4]); //numero di freccie

  if((N<10)||(N>30)||(A<1)||(A>3)||(I<3)||(I>6)){
    printf("NON consentito\n");
   exit(-1);
   }
 
   tiratore=(str_tiratore *)malloc(N*sizeof(str_tiratore));
 //inizializzazione semafori
    sem_init(&archi,0,A); //semaforo x gli archi
    sem_init(&freccie,0,I); //semaforo per le freccie
 //inizializzazione mutex
    pthread_mutex_init(&bersaglio, NULL);
 //inizializzazione dei thread
    pthread_attr_init(&attr);
     pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
   
    for(i=0; i<N; i++){
     tiratore[i].id=i;
      tiratore[i].parziale=0;
      tiratore[i].totale=0;
    rc=pthread_create(&tiratore[i].tid,&attr,routine_tiro,(void *)(&tiratore[i]));
      if(rc){
        printf("ERRORE.....\n");
         getchar(); //premere invio
     exit(-1);}
     }
 //rilascio delle risorse per i thread
    pthread_attr_destroy(&attr);
     for(i=0; i<N; i++)
        pthread_join(tiratore[i].tid,&status);  //si attende che finiscano tutti i thread

   printf("\n	CLASSIFICA	\n");
     for(i=0; i<N; i++){
    printf("tiratore %d, ha totalizzato %d punti\n",tiratore[i].id,tiratore[i].totale);
  }
 //rilascio delle risorse per il mutex
     pthread_mutex_destroy(&bersaglio);
 //rilascio delle risorse per i semafori
     sem_destroy(&archi);
       sem_destroy(&freccie);

pthread_exit(NULL);
 }
  
