#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>
#define PAZ 20

typedef struct{
  pthread_t tid;
  int id;
}str_cliente;

sem_t operatore;
pthread_mutex_t dentista;
pthread_t gestore;
int dent=0, assistente=0, dot=0, as=0;
void *routine_visita(void *arg){
  str_cliente *cliente;
   cliente=arg;
   int i;
    srand(time(NULL));
        while(1){
           usleep((rand()%10000)+10000);
         switch((rand()%4)+1){
            case 1: printf("Sono il cliente %d, devo fare solo la pulizia dei denti\n",cliente->id);
                     assistente=1;
                      as=1;
                       if(assistente==1)
                      pthread_exit(NULL);
                    printf("Sono il cliente %d, ho finito di fare la pulizia dei denti\n",cliente->id);
                       break;
            case 2: printf("Sono il cliente %d,ho bisogno solo del dentista\n",cliente->id);
                      
            case 3:  printf("Sono il cliente %d,devo fare entrambe le cose\n",cliente->id);
                      for(i=1; i<3; i++){
                         switch(i){
                           case 1:  printf("Sono il cliente %d,aspetto l'operatore\n",cliente->id);
                                      assistente=1;
                                      as=1;
                                     if(assistente==1)
                                      pthread_exit(NULL);
                                     printf("Sono il cliente %d, ho finito di fare la pulizia dei denti\n",cliente->id);
                                        break;
                          case 2: printf("Sono il cliente %d,attendo il dentista\n",cliente->id);
                                     dent=1;
                                      dot=1;
                                    if(dent==1)
                                    pthread_exit(NULL);
                                        printf("Sono il cliente %d, ho finito la visita\n",cliente->id);
                                     break;
                           }
                          }
          }
     }
}
void *routine_operatori(void *arg){
   while(1){
       if(as==1){
      sem_wait(&operatore);
        usleep((rand()%70000)+30000);
         sem_post(&operatore);
      printf("Sono un'assistente...mi riposo un po'\n");
     sem_wait(&operatore);
        usleep((rand()%20000)+10000);
       sem_post(&operatore);
    if(assistente==1)
       pthread_exit(NULL);
        }
     if(dot==1){
       pthread_mutex_lock(&dentista);
       usleep((rand()%70000)+30000);
      pthread_mutex_unlock(&dentista);
      printf("Sono il dottore...mi riposo un po'\n");
      pthread_mutex_lock(&dentista);
       usleep((rand()%20000)+10000);
     pthread_mutex_unlock(&dentista);
         if(dent==1)
       pthread_exit(NULL);
       }
    }
}
int main(int argc, char *argv[]){
 pthread_attr_t attr;
 int i, rc;
 void *status;
 str_cliente *cliente;

   if(argc!=1){
     printf("Errore...\n");
   exit(-1);
       }

   cliente=(str_cliente *)malloc(PAZ*sizeof(str_cliente));
   sem_init(&operatore,0,2);
   pthread_mutex_init(&dentista, NULL);

     pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for(i=0; i<PAZ; i++){
     cliente[i].id=i;
    rc=pthread_create(&cliente[i].tid,&attr,routine_visita,(void *)(&cliente[i]));
     if(rc){
        printf("Errore...\n");
      exit(-1);
       }
      }
   
     rc=pthread_create(&gestore,&attr,routine_operatori,NULL);
     if(rc){
        printf("Errore...\n");
      exit(-1);
       }
   
   pthread_attr_destroy(&attr);
    for(i=0; i<PAZ; i++)
      pthread_join(cliente[i].tid, &status);
  
   pthread_mutex_destroy(&dentista);
   
    sem_destroy(&operatore);

pthread_exit(NULL);
}
