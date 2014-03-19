#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>
#include<limits.h>

typedef struct{
  pthread_t tid;
  int id;
  int tempo;
}str_nuotatore;
sem_t corsia;

void *routine_nuoto(void *arg){
  str_nuotatore *tizio;
   tizio=arg;
   int i, j;
    srand(time(NULL));
      for(i=0; i<2; i++){
       for(j=0; j<4; j++){
        switch(j){
         case 0: printf("Sono il nuotatore %d, cerco di occupare una corsia x il nuoto a DORSO\n",tizio->id);
                  sem_wait(&corsia);
                   tizio->tempo+=(rand()%160)+40;
                  sem_post(&corsia);
                   break;
         case 1: printf("Sono il nuotatore %d, cerco di occupare una corsia x il nuoto a RANA\n",tizio->id);
                  sem_wait(&corsia);
                   tizio->tempo+=(rand()%160)+40;
                  sem_post(&corsia);
                   break;
         case 2: printf("Sono il nuotatore %d, cerco di occupare una corsia x il nuoto a STILE LIBERO\n",tizio->id);
                  sem_wait(&corsia);
                   tizio->tempo+=(rand()%160)+40;
                  sem_post(&corsia);
                   break;
         case 3: printf("Sono il nuotatore %d, cerco di occupare una corsia x il nuoto a DELFINO\n",tizio->id);
                  sem_wait(&corsia);
                   tizio->tempo+=(rand()%160)+40;
                  sem_post(&corsia);
                    break;
          }
        }
       }
pthread_exit(NULL);
}
int main(int argc, char *argv[]){
 str_nuotatore *tizio;
 int i, N, rc, min=INT_MAX, nome;
 void *status;
 pthread_attr_t attr;

  if(argc!=2){
    printf("Errore.....\n");
  exit(EXIT_FAILURE);
  }
  N=atoi(argv[1]);
  if((N<16)||(N>80)){
     printf("Errore.....\n");
  exit(EXIT_FAILURE);
  }
  tizio=(str_nuotatore *)malloc(N*sizeof(str_nuotatore));  //allocazione memoria per gli N amici
   //inizializzazione semaforo
      sem_init(&corsia,0,8);
   //inizializzazione thread
      pthread_attr_init(&attr);  //creazione degli attributi
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); //SETTO A JOINABLE

      for(i=0; i<N; i++){
       tizio[i].id=i;
       tizio[i].tempo=0;
      rc=pthread_create(&tizio[i].tid,&attr,routine_nuoto,(void *)(&tizio[i]));
       if(rc){
         printf("Errore.....\n");
     exit(EXIT_FAILURE);
     }  }
   //rilascio risorse
     pthread_attr_destroy(&attr);  //distruggo gli attributi
      for(i=0; i<N; i++)
       pthread_join(tizio[i].tid,&status);  //attendo il termine di tutti i thread

    printf("\n		CLASSIFICA		\n");
     for(i=0; i<N; i++)
        printf("Il nuotatore %d ha fatto un temppo di %d ms\n",tizio[i].id, tizio[i].tempo);
     for(i=0; i<N; i++){
        if(tizio[i].tempo<min){
          min=tizio[i].tempo;
          nome=tizio[i].id;
          }
       }
      printf("\nIl VINCITORE è il nuotatore %d, con tempo migliore %d ms\n", nome, min);
     sem_destroy(&corsia);

pthread_exit(NULL);
}
