#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<time.h>
#define STUDENTI 30

sem_t coca, aranciata, sprite, chinotto;
pthread_mutex_t distributore1,distributore2;
typedef struct{
   pthread_t tid;
   int id;
}str_studente;
void *routine_erogazione(void *arg){
   str_stdudente *studente;
   int c, a, s, ch;
   studente=arg;
      srand(time(NULL));
       usleep((rand()%31)+30);
       switch(rand()%2){
       case 0: printf("Sono lo studente %d, voglio la cocacola\n",studente->id);
                 pthread_mutex_lock(&distributore1);
                  sem_wait(&coca);
                 sem_getvalue(&coca,&c);
                 if(c!=0)
                   usleep(((rand()%3)+2)*1000);
                 if(c==0) case 1;
                  
       case 1:
        }
}
int main(int argc, char *argv[]){
 int i, rc;
 str_studente *studente;
 pthread_attr_t attr;
 void *status;

   if(argc!=1){
     printf("Errore!!\n");
    exit(-1);
    }

  studente=(str_studente *)malloc(STUDENTI*sizeof(str_studente)); //allocazione dinamica
   //inizializzazione semafori
     sem_init(&coca,0,50);
     sem_init(&aranciata,0,20);
     sem_init(&sprite,0,20);
     sem_init(&chinotto,0,10);
   //inizializzazione mutex
     pthread_mutex_init(&distributore1);
     pthread_mutex_init(&distributore2);
  //inizializzazione thread
    pthread_attr_init(&attr);
     pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

     for(i=0; i<STUDENTI;i++){
      studente[i].id=i;
       rc=pthread_create(&studente[i].tid,&attr,routine_erogazione,(void *)(&studente[i]));
        if(rc){
         printf("ERRORE!!\n");
        exit(-1);
      }
   }
  //rilascio risorse
     pthread_attr_destroy(&attr);
      for(i=0; i<STUDENTI; i++)
       pthread_join(studente[i].tid,&status);  //attende la fine di tutti i thread
     //rilascio risorse mutex
       pthread_mutex_destroy(&distributore1);
       pthread_mutex_destroy(&distributore2);
   //rilascio delle risorse x i semafori
     sem_destroy(&coca);
     sem_destroy(&aranciata);
     sem_destroy(&sprite);
     sem_destroy(&chinotto);

pthread_exit(NULL);
}
