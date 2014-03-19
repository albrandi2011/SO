/*
* per compilare: gcc occorrenze_cifre.c -Wall -O3 -o occorrenze_cifre
* per eseguire: ./occorrenze_cifre <num intero>
* cosa fa: determina la cifra più frequente in un intero dato in input,
* per esempio eseguendo ./occorrenze_cifre 489116811 verrà visualizzato
* in output "La cifra 1 ha avuto maggiori occorrenze, ovvero: 4"
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define N 10
typedef struct{//definisco la struttura figlio che contiene 
	int occ;//un intero per le occorrenze del numero corrispondente
	int fpipe[2];//e una pipe per dialogare col padre
}str_figli;
int main (int argc, char* argv[]){
int i,j,len,st,max=0;
char s[100];
pid_t okfork;
str_figli *figli;
if(argc!=2){
	printf("Parametri errati!");
	exit(0);
}
figli=(str_figli*)malloc(N*sizeof(str_figli));//creo un vettore di figli
for(i=0;i<10;i++){//e creo le pipe
	if(pipe(figli[i].fpipe)<0)
		exit(1);
}
sprintf(s,"%s",argv[1]);
//sprintf(s,"%s","114561211355");
len= strlen(s);
for(i=0;i<10;i++){
	if((okfork=fork())<0){
		printf("ERRORE FORK");
		exit(0);
	}
	if(okfork==0){//per ogni figlio
		figli[i].occ=0;//inizializzo il contatore delle occorrenze
		close(figli[i].fpipe[0]);//chiudo il canale di lettura della pipe
		for(j=0;j<len;j++){
			if(s[j]==(char)(48+i)){//e incremento il contatore
				figli[i].occ++;//se trovo dei numeri corrispondenti
			}
		}//e infine scrivo sulla pipe il numero
		write(figli[i].fpipe[1],&figli[i].occ,sizeof(int));
		exit(1);
	}else{
		close(figli[i].fpipe[1]);//il padre chiude in scrittura la pipe
		wait(&st);//attende che l'i-esimo figlio finisca il conteggio
		read(figli[i].fpipe[0],&figli[i].occ,sizeof(int));//e conserva il numero di occorrenze
	}
}
for(i=0;i<10;i++){//individua il numero massimo di occorrenze
	if(figli[i].occ>max){
		max=figli[i].occ;
	}
}
for(i=0;i<10;i++){//e determina le cifre con quella frequenza
	if(figli[i].occ==max){
		printf("La cifra %d ha avuto maggiori occorrenze, ovvero: %d\n",i,figli[i].occ);
	}
}
exit(1);
}
