//Sergio Caruso M: 0538499
//Nel seguente programma il numero di gruppi che devono fare uso della pista è scelto come define, il programma 
//quindi termina quando tutti i gruppi hanno usato la pista.
//nel programma sono stati usati 2 semafori uno per gestire i go-kart, inizializzato a G (numero di go-kart dati
// in input), e un'altro che controlla l'accesso alla pista. Il programma funziona in questa maniera: 
//al kartodromo arrivano gruppi di persone in cui il numero delle persone è scento in maniera random, quindi 
//inizializzamo la struttura "corridori" a N e dopo aver settato gli attributi ad ogni persona lanciamo i vari
//thread associati ad ogni persona del gruppo, nella funzione associata ai thread l'accesso alla pista viene
//gestita dal semaforo "mezzi" (inizializzato a G) quindi si può accedere in pista quando tale semaforo è 
//a zero (il sem_wait ha portato "mezzi" a 0), mettendo in attesa le restanti N-G persone, oppure quando si 
//devono smaltire le persone restanti nello stesso gruppo e in questo caso il numero delle persone restanti
//si controlla tramite la variabile "utenti".
//il programma segue questo iter per tutti i gruppi di persone.
#include<stdio.h>
#include<unistd.h>
#include<malloc.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<time.h>
#define gruppi 3

int G,N,t=0;
int uso_pista=0; //utilizzata per calcolare il numero di persone che accedono alla pista
int T=1; //tempo del giro

struct corridori{ 
int id;
int gruppo;
pthread_t tid;
}*persone;

sem_t mezzi,pista;

void *giro(void *attr)
{

 sem_wait(&mezzi);
 int val,k,utenti=0; 
 int j=*(int *)attr;
 sem_getvalue(&mezzi,&val);
 utenti++;
 printf("(Gruppo %d) la persona %d prende un go-kart\n",persone[j].gruppo,persone[j].id);  
   if (val==0 || utenti==N) //gli utenti possono accedere alla pista solo se il semaforo e a zero 
   							//oppure quando il numero degli utenti arriva a N 
      {
 
      printf("Le persone possono partire\n");
      sem_wait(&pista); 
      printf("Le persone sono in pista che corrono\n");
      sleep(T);
      if(val==0)
        {
        for(k=0;k<G;k++)
          sem_post(&mezzi);
        }
      sem_post(&pista);
      printf("Le persone hanno liberato i go-kart e la pista\n");  
       }
    //il sem_post nel caso in cui si devono smaltire i restanti utenti non viene eseguito 
    //perchè al massimo ogni gruppo può fare 2 giri e di conseguenza dopo il aver effettuato 
    //il primo non occorre fare il sem_post perchè tale semaforo verra inizializzato nel main come 
    //exnovo
}

int main()
{
 int i;

 //non viene effettuato il controllo del numero di tempo per mancanza di tempo 
 //e ci si affida alla bontà dell'utente 
 printf("Inserire numero di go-kart da usare (tale numero deve essere pari)\n");
 scanf("%d",&G); 
 
 
 while(t<gruppi) //per tutti i gruppi
 {
  N=(rand()%(2*G))+1; //scelta delle persone del gruppo
  
  printf("Il gruppo %d formato da %d persone arriva al kartodromo\n",t,N);
  uso_pista=uso_pista+N; //si aggiorna la variabile che tiene traccia del numero di persone che accedono in pista
  
  sem_init(&mezzi,0,G);
  sem_init(&pista,0,1);
  
  persone=(struct corridori *)malloc(N*sizeof(struct corridori *));
  
  for (i=0;i<N;i++)
  {
   persone[i].id=i;
   persone[i].gruppo=t;
   
   pthread_create(&persone[i].tid,NULL,giro,(void *)&persone[i].id);

  }
  
  sleep(3);
  
 // for (i=0;i<N;i++)
   // pthread_join(persone[i].tid,NULL);
  
//l'allocazione dinamica della struttura ad ogni iterazione fa sballare il pthread_join e di conseguenza
//si ottiene un errore di segmentazione perchè N non è constante per tutta l'esecuzione del programma
//un'alternativa sarebbe quella di utilizzare join separati ad ogni iterazione salvando il numero di
//persone in un opportuno array   
  t++;
  }
 printf("Tutti i gruppi di persone hanno girato in pista\n");
 printf("La pista è stata utilizzata da %d persone\n",uso_pista); 
}


