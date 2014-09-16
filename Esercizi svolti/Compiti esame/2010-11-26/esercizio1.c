#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#define PAZIENTE 0
#define DONATORE 1

pthread_mutex_t *lettini, *sacche;
pthread_t gestore;
int L, sac, let;
int termina=0;
typedef struct{
  pthread_t tid;
  int id;
  int key;
}str_persona;
void *routine_prelievo(void *arg){
  str_persona *persona;
  persona=arg;
   
               while(1){
                
               let=rand()%L;
              pthread_mutex_lock(&lettini[let]);
             printf("Sono la persona %d, ho occupato un lettino\n",persona->id);
     
              pthread_mutex_lock(&sacche[let]);
               printf("Sono la persona %d, Sto donando\n",persona->id);
                sleep(rand()%7);
                   termina=1;
                  if(termina==1){
                   pthread_exit(NULL);}
               
                sleep(rand()%7);
               pthread_mutex_unlock(&lettini[let]);
                printf("Vado via %d...\n",persona->id);
              
           }
}
void *routine_gestore(void *arg){
  
   while(1){
    pthread_mutex_unlock(&sacche[sac]);
    printf("Ho liberato il paziente....\n");
    if(termina==1)
    pthread_exit(NULL);
   }
  termina=0;
}
int main(int argc, char *argv[]){
 int don, rc, i;
  str_persona *persona;
  pthread_attr_t attr;
  void *status;

  srand(time(NULL));
  if(argc!=3){
   printf("Errore!!\n");
    exit(-1);
    }

 don=atoi(argv[1]); //numero di persone
 L=atoi(argv[2]); //numero lettini e sacche

   persona=(str_persona *)malloc(don*sizeof(str_persona));
  lettini=(pthread_mutex_t *)malloc(L*sizeof(pthread_mutex_t));
  sacche=(pthread_mutex_t *)malloc(L*sizeof(pthread_mutex_t));
   //inizializzazione mutex
   for(i=0; i<L; i++){
      pthread_mutex_init(&lettini[i],NULL);
      pthread_mutex_init(&sacche[i],NULL);
        }
   //inizializzo il thread per il medico
      
  //inizializzazione thread
    pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for(i=0; i<don; i++){
      persona[i].id=i;
      persona[i].key=rand()%2;
      rc= pthread_create(&persona[i].tid,&attr,routine_prelievo,(void *)(&persona[i]));
    if(rc){
     printf("Errore!!\n");
      exit(-1);
       }
     }
    pthread_create(&gestore,&attr,routine_gestore,NULL);
    pthread_attr_destroy(&attr);
      for(i=0; i<don; i++){
       pthread_join(persona[i].tid,&status);
        }
  //rilascio risorse
    for(i=0; i<L; i++){
       pthread_mutex_destroy(&lettini[i]);
       pthread_mutex_destroy(&sacche[i]);
        }

pthread_exit(NULL);
}
