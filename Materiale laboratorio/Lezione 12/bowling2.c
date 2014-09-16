#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

#define MAX_GIOC 21
#define MIN_GIOC 8
#define MAX_TIRI 30
#define MIN_TIRI 10


void *azione_gioc(void *arg);

typedef struct {
    int punti;
    int id;
    pthread_t pid;
} DATA_PUN;

DATA_PUN giocatore[MAX_GIOC];

pthread_mutex_t campo;

sem_t palle_rosse;
sem_t palle_nere;
sem_t palle_verdi; 

pthread_attr_t attr;
int N,K;

void *azione_gioc(void *arg)
{
   DATA_PUN *miodato;
   int i, palla = rand() % 3;
   miodato = arg;
   printf("Sono il giocatore %d e ho scelto la palla di tipo %d\n", miodato->id, palla);
   for(i=0; i<N; i++) {                    //ogni giocatore sceglie la palla (se non è disponibile attende)
      switch(palla)
        {case 0: sem_wait(&palle_verdi); break;
         case 1: sem_wait(&palle_rosse); break;
         case 2:sem_wait(&palle_nere); break;
        }
   printf("Sono il giocatore %d e ho preso  la palla %d\n",miodato->id,palla);  //una volta scelta la palla blocca il campo(lock) per effettuare il tiro
   pthread_mutex_lock (&campo);

   printf("Sono il giocatore %d sto per tirare\n",miodato->id,palla);  
   miodato->punti+=rand()%11;   //effettua il tiro
    
   printf("Sono il giocatore %d fino al tiro %d  ho fatto %d \n",miodato->id,i,miodato->punti);  
   pthread_mutex_unlock (&campo);     // libera il campo(unlock)

   sleep(1);

   switch(palla)  //libera la palla che aveva scelto
       {case 0: sem_post(&palle_verdi); break;
         case 1: sem_post(&palle_rosse); break;
         case 2:sem_post(&palle_nere); break;
       }
   }
   pthread_exit((void*) 0);  //dopo che tutti i giocatori hanno effettuato i loro tiri termina la funzione
}



int main(int argc, char *argv[])
{   

  long i;
  void *status;   //serve per la funzione join
  srand(time(NULL));
  pthread_attr_t attr;


    
    // controllo se gli argomenti sono stati inseriti in maniera corretta e in caso contrario esco
    if (argc != 3) {
         printf("inserisci gli argomenti in maniera corretta\n");
         exit(-1);
    }

    //assegno i valori degli argomenti
    N = atoi(argv[1]);  //numero di giocatori
    K = atoi(argv[2]);  //numero di tiri

        // controllo se gli argomenti sono stati inseriti in maniera corretta e in caso contrario esco
    if ((N < MIN_GIOC) || (N > MAX_GIOC) || (K < MIN_TIRI) || (K > MAX_TIRI)) {
         printf("inserisci gli argomenti in maniera corretta\n");
         exit(-1);
    }

    
    // inizializzo i semafori
    sem_init(&palle_rosse, 0, 2);
    sem_init(&palle_nere, 0, 3);
    sem_init(&palle_verdi, 0, 2);

    //inizializzo il mutex
    pthread_mutex_init(&campo, NULL);

    //inizializzo i giocatori e i rispettivi punteggi
    for(i=0; i<N; i++) {
       giocatore[i].punti = 0;
       giocatore[i].id = i;
    }
    
    //inizializzo gli attributi e li setto in modo da poter creare dei thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //inizializzo i thread
    for(i=0; i<N; i++) {
       pthread_create(&(giocatore[i].pid), &attr, azione_gioc, (void *) (&giocatore[i]));
    }

    //elimino gli attributi
    pthread_attr_destroy(&attr);

    //aspetto che tutti i thread teminino
    for(i=0; i<N; i++) {
       pthread_join(giocatore[i].pid, &status);   //nel join senza &
    }
    
    //stampo i risultati
    for(i=0; i<N; i++) {
       printf("Il giocatore %d ha ottenuto un punteggio finale di %d\n", giocatore[i].id, giocatore[i].punti);
    }

    //dealloco le risorse
    sem_destroy (&palle_verdi);
    sem_destroy (&palle_rosse);
    sem_destroy (&palle_nere);

    pthread_mutex_destroy(&campo);

    pthread_exit(NULL);
}  



