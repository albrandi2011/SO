#include <stdio.h>                  //con i tempi dell'esercizio il programma si blocca (il gestore non riesce a bloccare i mutex)
#include <pthread.h>                //dopo 30 sec si avvia la terminazione del programma
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>

#define N 30

void *studente_routine(void *arg);
void *gestore_routine(void *);

//due mutex per i distributori
pthread_mutex_t distributori[2];



//semafori per ogni tipo di bibita
sem_t coca, chino, fanta, sprite;

//thread per gli studenti e per il gestore
pthread_t gestore;

typedef struct {
  pthread_t tid;
  int nome;
  int ccoca;
  int csprite;
  int cfanta;
  int cchino;
} str_studente;

int s_coca, s_sprite, s_fanta, s_chino;  // t = tempo; s = variabili per controllare i valori dei semafori
int finito = 0;

void *studente_routine(void *arg)
{
   int i,t_alunno,t_erogazione;
   int scelta;
   str_studente *alunno;
   srand(time(NULL));

   alunno = arg;
   t_alunno = ((rand() % 30) + 31) * 1000;
   t_erogazione = ((rand() % 3) + 2) * 1000;   




   while(1) {
         sleep(1);
         printf("Sono l'alunno %d! Ho sete\n", alunno->nome);
         sem_getvalue(&coca, &s_coca);
         sem_getvalue(&sprite, &s_sprite);
         sem_getvalue(&fanta, &s_fanta);
         sem_getvalue(&chino, &s_chino);
         if ((s_coca != 0) || (s_sprite != 0) || (s_fanta != 0) || (s_chino != 0)) {
              scelta = (rand() % 4) + 1;                       // 1= coca, 2= sprite, 3= fanta, 4= chino;
              switch (scelta) {
                      case 1: printf("Sono l'alunno %d! voglio una COCA!\n", alunno->nome);
                              
                              if (s_coca != 0) {
                                  pthread_mutex_lock(&distributori[0]);
                                  usleep(t_erogazione);
                                  sem_wait(&coca);
                                  pthread_mutex_unlock(&distributori[0]);
                                  printf("Sono l'alunno %d! Che biedda sta COCA COLA!\n", alunno->nome);
                                  alunno->ccoca++;
                              }
                              else {
                                  
                                  scelta = (rand() % 3) + 1;
                                  switch (scelta) {
                                          case 2: printf("Sono l'alunno %d! I COCA COLA fineru! Mi futtu na SPRITE\n", alunno->nome);
                                                  
                                                  if(s_sprite != 0) {
                                                     pthread_mutex_lock(&distributori[1]);
                                                     usleep(t_erogazione);
                                                     sem_wait(&sprite);
                                                     pthread_mutex_unlock(&distributori[1]);
                                                     printf("Sono l'alunno %d! Che biedda sta SPRITE!\n", alunno->nome);
                                                     alunno->csprite++;                                                    
                                                  }
                                                  else {
                                                      
                                                      printf("Sono l'alunno %d! Fineru puru i SPRITE! Buttana ra regina!!!\n", alunno->nome);
                                                  }
                                          break;
                                          case 3: printf("Sono l'alunno %d! I COCA COLA fineru! Mi futtu na FANTA\n", alunno->nome);
                                                  
                                                  if(s_fanta != 0) {
                                                     pthread_mutex_lock(&distributori[1]);
                                                     usleep(t_erogazione);
                                                     sem_wait(&fanta);
                                                     pthread_mutex_unlock(&distributori[1]);
                                                     printf("Sono l'alunno %d! Che biedda sta FANTA!\n", alunno->nome);
                                                     alunno->cfanta++;                                                    
                                                  }
                                                  else {
                                                      
                                                      printf("Sono l'alunno %d! Fineru puru i FANTA! Buttana ra regina!!!\n", alunno->nome);
                                                  }
                                          break;
                                          case 4: printf("Sono l'alunno %d! I COCA COLA fineru! Mi futtu un CHINOTTO\n", alunno->nome);
                                                  
                                                  if(s_chino != 0) {
                                                     pthread_mutex_lock(&distributori[1]);
                                                     usleep(t_erogazione);
                                                     sem_wait(&chino);
                                                     pthread_mutex_unlock(&distributori[1]);
                                                     printf("Sono l'alunno %d! Che bieddu stu CHINOTTO!\n", alunno->nome);
                                                     alunno->cchino++;                                                    
                                                  }
                                                  else {
                                                      
                                                      printf("Sono l'alunno %d! Fineru puru i CHINOTTI! Buttana ra regina!!!\n", alunno->nome);
                                                  }
                                          break;
                                  }
                              }
                      case 2: printf("Sono l'alunno %d! voglio una SPRITE!\n", alunno->nome);
                              
                              if (s_sprite != 0) {
                                  pthread_mutex_lock(&distributori[1]);
                                  usleep(t_erogazione);
                                  sem_wait(&sprite);
                                  pthread_mutex_unlock(&distributori[1]);
                                  printf("Sono l'alunno %d! Che biedda sta SPRITE!\n", alunno->nome);
                                  alunno->csprite++;
                              }
                              else {
                                  
                                  printf("Sono l'alunno %d! I SPRITE fineru! Mi futtu na COCA COLA\n", alunno->nome);
                                                  
                                                  if(s_coca != 0) {
                                                     pthread_mutex_lock(&distributori[0]);
                                                     usleep(t_erogazione);
                                                     sem_wait(&coca);
                                                     pthread_mutex_unlock(&distributori[0]);
                                                     printf("Sono l'alunno %d! Che biedda sta COCA COLA!\n", alunno->nome);
                                                     alunno->ccoca++;                                                    
                                                  }
                                                  else {
                                                      
                                                      printf("Sono l'alunno %d! Fineru puru i COCA COLA! Buttana ra regina!!!\n", alunno->nome);
                                                  }
                              }
                      break;
                      case 3: printf("Sono l'alunno %d! voglio una FANTA!\n", alunno->nome);
                              
                              if (s_fanta != 0) {
                                  pthread_mutex_lock(&distributori[1]);
                                  usleep(t_erogazione);
                                  sem_wait(&fanta);
                                  pthread_mutex_unlock(&distributori[1]);
                                  printf("Sono l'alunno %d! Che biedda sta FANTA!\n", alunno->nome);
                                  alunno->cfanta++;
                              }
                              else {
                                  
                                  printf("Sono l'alunno %d! I FANTA fineru! Mi futtu na COCA COLA\n", alunno->nome);
                                                  
                                                  if(s_coca != 0) {
                                                     pthread_mutex_lock(&distributori[0]);
                                                     usleep(t_erogazione);
                                                     sem_wait(&coca);
                                                     pthread_mutex_unlock(&distributori[0]);
                                                     printf("Sono l'alunno %d! Che biedda sta COCA COLA!\n", alunno->nome);
                                                     alunno->ccoca++;                                                    
                                                  }
                                                  else {
                                                      
                                                      printf("Sono l'alunno %d! Fineru puru i COCA COLA! Buttana ra regina!!!\n", alunno->nome);
                                                  }
                              }
                      break;
                      case 4: printf("Sono l'alunno %d! voglio un CHINOTTO!\n", alunno->nome);
                              
                              if (s_chino != 0) {
                                  pthread_mutex_lock(&distributori[1]);
                                  usleep(t_erogazione);
                                  sem_wait(&chino);
                                  pthread_mutex_unlock(&distributori[1]);
                                  printf("Sono l'alunno %d! Che bieddu stu CHINOTTO!\n", alunno->nome);
                                  alunno->cchino++;
                              }
                              else {
                                  
                                  printf("Sono l'alunno %d! I CHINOTTI fineru! Mi futtu na COCA COLA\n", alunno->nome);
                                                  
                                                  if(s_coca != 0) {
                                                     pthread_mutex_lock(&distributori[0]);
                                                     usleep(t_erogazione);
                                                     sem_wait(&coca);
                                                     pthread_mutex_unlock(&distributori[0]);
                                                     printf("Sono l'alunno %d! Che biedda sta COCA COLA!\n", alunno->nome);
                                                     alunno->ccoca++;                                                    
                                                  }
                                                  else {
                                                      
                                                      printf("Sono l'alunno %d! Fineru puru i COCA COLA! Buttana ra regina!!!\n", alunno->nome);
                                                  }
                              }
                      break;    
              }  
         }
         else {
            printf("Sono l'alunno %d! Si futteru tuttu!!!\n",alunno->nome);
            
         }
         if (finito==1)
             pthread_exit(NULL);
   }
    
}

void *gestore_routine(void *arg)
{
     int j;
     while(1) {
         usleep(10);
         printf("SONO IL GESTORE! RICARICO TUTTO!\n");
         pthread_mutex_lock(&distributori[1]);

         sem_getvalue(&sprite, &s_sprite);
         sem_getvalue(&fanta, &s_fanta);
         sem_getvalue(&chino, &s_chino);

         
         for (j=0;j<(20-s_sprite);j++)
              sem_post(&sprite);
         for (j=0;j<(20-s_fanta);j++)
              sem_post(&fanta);
         for (j=0;j<(20-s_chino);j++)
              sem_post(&chino);
         pthread_mutex_unlock(&distributori[1]);
  
         pthread_mutex_lock(&distributori[0]);
         sem_getvalue(&coca, &s_coca);
         for (j=0;j<(50-s_coca);j++) 
              sem_post(&coca);

         pthread_mutex_unlock(&distributori[0]);

         sleep(1);

         
            if (finito == 1)
             pthread_exit(NULL);
                  

     }

}


void main(int argc, char *argv[])
{
   
   int i, rc;
   pthread_attr_t attr;
   str_studente studente[N];

   pthread_mutex_init(&distributori[0], NULL);
   pthread_mutex_init(&distributori[1], NULL);
       // inizializzo i semafori
    sem_init(&coca, 0, 50);
    sem_init(&chino, 0, 10);
    sem_init(&fanta, 0, 20);
    sem_init(&sprite, 0, 20);
    
    //inizializzo thread per il gestore
    pthread_create(&gestore, NULL, gestore_routine, NULL);
    
    //inizializzo gli attributi per i thread studenti e li setto joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //inizializzo i thread studente
    for (i=0; i<N; i++) {
        studente[i].nome = i;
        studente[i].ccoca = 0;
        studente[i].csprite = 0;
        studente[i].cfanta = 0;
        studente[i].cchino = 0;
        rc = pthread_create(&studente[i].tid, &attr, studente_routine, (void *) &studente[i]);
        if (rc) {
            printf("si è verificato un errore\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_attr_destroy(&attr);
 

 sleep(30);            //dopo 30 secondi si attiva la terminazione del programma!
finito= 1;

    for(i=0; i<N; i++) {
    pthread_join(studente[i].tid, NULL);
    }

    for (i=0; i<N; i++) {
          printf("Sono lo studente %d! Ho bevuto %d coca cole, %d sprite, %d fanta, %d chinotti!\n", studente[i].nome, studente[i].ccoca, studente[i].csprite, studente[i].cfanta, studente[i].cchino);
    }



    sem_destroy(&coca);
    sem_destroy(&sprite);
    sem_destroy(&fanta);
    sem_destroy(&chino);

    pthread_mutex_destroy(&distributori[0]);
    pthread_mutex_destroy(&distributori[1]);

   pthread_exit(NULL);    


}
