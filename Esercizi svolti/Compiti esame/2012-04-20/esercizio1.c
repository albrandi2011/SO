#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>
#include<limits.h>

typedef struct{
  pthread_t tid;
  int id;
  int punti;
  int palla;
}str_giocatore;
int K;
sem_t verde, rossa, nera;  //uso i semafori per le palle
pthread_mutex_t pista;     //uso la mutua esclusione per la pista

void *routine_tiro(void *arg){
   str_giocatore *tizio;
   tizio=arg;
   int i;
    srand(time(NULL));
           for(i=0; i<K; i++){
           switch(rand()%2){
             case 0: printf("Sono il giocatore %d, ho scelto la palla verde, cerco di prenderla\n",tizio->id);
                      sem_wait(&verde); 
                      tizio->palla=0;break;
             case 1: printf("Sono il giocatore %d, ho scelto la palla rossa, cerco di prenderla\n",tizio->id);
                      tizio->palla=1;sem_wait(&rossa); break;
             case 2: printf("Sono il giocatore %d, ho scelto la palla nera, cerco di prenderla\n",tizio->id);
                      tizio->palla=2;sem_wait(&nera); break; }
              printf("Sono il giocatore %d, mi metto a turno x tirare\n", tizio->id);
               pthread_mutex_lock(&pista);
             tizio->punti+=rand()%10;
               printf("Sono il giocatore %d,ho fatto %d punti\n",tizio->id,tizio->punti);
                pthread_mutex_unlock(&pista);
               switch(tizio->palla){
             case 0:  sem_post(&verde); break;
             case 1:  sem_post(&rossa); break;
             case 2:  sem_post(&nera); break; }
               sleep(4); 
     }
   pthread_exit(NULL);
}
int main(int argc, char *argv[]){
  str_giocatore *tizio;
  int N, i, rc;
  pthread_attr_t attr;
  void *status;

     if(argc!=3){
       printf("Hai inserito un numero sbagliato di parametri..ne sono previsti 2\n");
      getchar();
      exit(EXIT_FAILURE);
        }
   N=atoi(argv[1]);  //numero di persone che devono giocare
   K=atoi(argv[2]);  //numero di tiri
  
        if((N<8)||(N>21)||(K<10)||(K>30)){
            printf("I valori inseriti NON sono consentiti....\n");
      getchar();
      exit(EXIT_FAILURE);
       }

   tizio=(str_giocatore *)malloc(N*sizeof(str_giocatore));   //allocazione dinamica della memoria per i N amici
    
     //inizializzazione semafori
        sem_init(&verde,0,2);  //palle di colore verde
        sem_init(&rossa,0,2);  //palle di colore rosso
        sem_init(&nera,0,3);   //palle di colore nero

   //inizializzazione mutex
       pthread_mutex_init(&pista, NULL);
 
   //inizializzazione thread
       pthread_attr_init(&attr);   //inizializzazione attributi
     pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);   //li setto joinable

       for(i=0; i<N; i++){
         tizio[i].id=i;
         tizio[i].punti=0;
       rc=pthread_create(&tizio[i].tid,&attr,routine_tiro,(void *)(&tizio[i]));
          if(rc){
         printf("ERRORE.........\n");
           exit(EXIT_FAILURE);
       }
      }

    pthread_attr_destroy(&attr);  //distruggo gli attributi
   
       for(i=0; i<N; i++)
        pthread_join(tizio[i].tid, &status);  //attendo che finiscano tutti i thread
      
     printf("\n		CLASSIFICA	\n");
       for(i=0; i<N; i++){
         printf("Il giocatore %d ha totalizzato %d punti\n",tizio[i].id, tizio[i].punti);
     }
   //rilascio risorse da parte del mutex
      pthread_mutex_destroy(&pista);

    sem_destroy(&verde);  
     sem_destroy(&rossa);
    sem_destroy(&nera);

  pthread_exit(NULL);
}
