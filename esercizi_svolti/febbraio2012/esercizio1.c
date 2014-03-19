#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>
#define ATL 17
#define A 0
#define B 1

//utilizzo un semaforo per le 8 corsie, e la mutua esclusione per la pedana
sem_t corsia;
pthread_mutex_t pedana;
//struttura che rappresenta ogni atleta
typedef struct{
  pthread_t tid;
  int id;
  int gruppo;
  int punt_corsa;
  int punt_salto;
}str_atleta;

void *routine_gara(void *arg){
  str_atleta *atleta;
  atleta=arg;
  int i, x, j;
     srand(time(NULL));
           for(i=0; i<2; i++){
             if(atleta->gruppo==0){
              printf("Sono l'atleta %d, vado a fare la corsa\n",atleta->id);
                       sem_wait(&corsia);
                        x=(rand()%20000)+80000;
                       usleep(x);
                       sem_post(&corsia);
                      atleta->punt_corsa=100-(x/1000);
                   }
                if(atleta->gruppo==1){       
              printf("Sono l'atleta %d, cerco di occupare la pedana\n",atleta->id);
                      pthread_mutex_lock(&pedana);
                      for(j=1; j<4; j++){
                        printf("Sono l'atleta %d, faccio il salto numero %d\n",atleta->id,j);
                         usleep(100000);
                          atleta->punt_salto+=(rand()%20);
                           }
                        pthread_mutex_unlock(&pedana);
                     }
                  sleep(2);
                 if(atleta->gruppo==0)
                   atleta->gruppo=1;
                  else
                   atleta->gruppo=0;
                 }
 pthread_exit((void *)0);
}
int main(int argc, char *argv[]){
 str_atleta *atleta;
 int i, rc;
 void *status;
  pthread_attr_t attr;

   if(argc!=1){
      printf("Errore.....\n");
     getchar();   //premere invio
   exit(EXIT_FAILURE);
   }

  atleta=(str_atleta *)malloc(ATL*sizeof(str_atleta)); //allocazione dinamica della memoria per i 16 atleti

  //inizializzazione semaforo
     sem_init(&corsia,0,8);
  //inizializzazione mutex
     pthread_mutex_init(&pedana, NULL);

   //inizializzazione thread
      pthread_attr_init(&attr);                            //inizializzo gli attributi e li setto joinable
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);  
    
       for(i=1; i<ATL; i++){
         atleta[i].id=i;
         atleta[i].punt_corsa=0;
         atleta[i].punt_salto=0;
         atleta[i].gruppo=0;  //gruppo A
        if(i>=8)
         atleta[i].gruppo=1;   //gruppo B

      rc=pthread_create(&atleta[i].tid,&attr,routine_gara,(void *)(&atleta[i]));
         if(rc){
            printf("Errore.....\n");
              getchar();   //premere invio
         exit(EXIT_FAILURE);}
               }

     //rilascio delle risorse 
        pthread_attr_destroy(&attr);  //distruggo gli attributi
         
          for(i=1; i<ATL; i++)
           pthread_join(atleta[i].tid, &status);  //attende che finiscano tutti i thread

         printf("\n	CLASSIFICA	\n");
         for(i=1; i<ATL; i++)
        printf("L'atleta %d, ha ottenuto %d nel salto e %d nella corsa\n",atleta[i].id,atleta[i].punt_salto,atleta[i].punt_corsa);

    //rilascio risorse da parte del mutex
       pthread_mutex_destroy(&pedana);

    sem_destroy(&corsia);
 pthread_exit(NULL);
}
