/*
* per compilare: gcc fork_signal.c -Wall -O3 -o fork_signal
* per eseguire: ./fork_signal a1 a2 a3... 
* (con "a" intero, almeno 2 numeri interi)
* cosa fa: il processo padre genera tanti figli quanti sono i valori 
* dati in input. A partire dall'ultimo figlio, si calcola t=an+a(n-1) 
* e si ferma per t secondi, dopo di che sveglia il figlio che lo precede
* e ripete le stesse operazioni fino al termine dei figli.
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
void sveglia_figlio(int signum);
void sveglia_figlio(int signum){
	//questo segnale mi serve per svegliare i figli addormentati
}


int main (int argc, char *argv[]){
	
	int status,i,t,nnum;
	pid_t* npid;
	int* num;
	//mi servono almeno 2 numeri interi
	if(argc<3){
		printf("parametri non corretti");
		exit(0);
	}
	//determino la dimensione
	nnum=argc-1;
	//e creo un array per i pid
	npid=(pid_t*)malloc(nnum*sizeof(pid_t));
	//e uno con gli interi da calcolare
	num=(int*)malloc(nnum*sizeof(int));
	//recupero i valori dai parametri ricevuti
	for(i=0;i<nnum;i++){
		num[i]=atoi(argv[i+1]);
	}
	//conservo il vecchio segnale per poi ripristinarlo	
	void(*signal_old)(int);	
	signal_old=signal(SIGUSR1,sveglia_figlio);
	//cominciamo con il ciclo per effettuare nnum fork
	for(i=0;i<nnum;i++){
		npid[i]=fork();
		//se sono figlio vado subito a riposo(pause)
		if(npid[i]==0){
			pause();
			//se ricevo un segnale riparto da qui
			if(i>0){
				//calcolo la somma del mio numero col precedente
				t=num[i]+num[i-1];
				printf("Figlio %d, aspetto %d secondi e termino...\n",getpid(),t);
				//e aspetto t secondi
				sleep(t);
				//dopo di che invio il segnale al figlio che mi precede
				kill(npid[i-1],SIGUSR1);
				//e termino
				exit(1);
			}else{
				exit(1);
			}
			
		}else{
			//se sono il padre, se ho generato tutti i figli mando il segnale all'ultimo
			if(i==(nnum-1)){
			printf("Ho generato tutti i figli, adesso lancio l'ultimo!\n");
			kill(npid[i],SIGUSR1);
			}
		}
	}
	//attendo che tutti i figli finiscano il loro compito
for(i=0; i<nnum;i++){
      wait(&status);
     }
printf("Ok, tutto bene!\n");
//ripristino il segnale
signal(SIGUSR1,signal_old);
//e termino
exit(1);
}
