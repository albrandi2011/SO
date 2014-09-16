## Funzioni processi

pid_t fork (void) 
	esempio: fork ( )
	- Biforca il processo in padre e figlio: al figlio restituisce sempre 0;
	- al padre restituisce il pid > 0 del figlio biforcato; 
	- restituisce -1 se la biforcazione del processo fallisce.

void exit (int stato)
	esempio: exit() 
	- (per il momento senza parametro == exit(0))
	- Termina il processo corrente
	- Simile alla return: pu� essere superflua, 
	e.g. se il programma giunge alla fine del codice 


```c
/****************************** CODICE **************************************/
#include <stdio.h>
#include <sys/types.h>
void main( )
{	
	pid_t pid;	
	pid=fork( );
	if (pid==-1) {printf(�errore esecuzione fork�); exit();}
		else
	if (pid==0)
		{printf("sono il processo figlio\n");
		 exit( );
		 }
	else 	{printf("sono il processo padre\n");
		 exit( ); /* non necessaria */
		 }
}
/***************************************************************************/
```


pid_t getpid ()
	- Restituisce al processo che la chiama il valore del suo pid


```c
/***************************** CODICE **************************************/
#include <stdio.h>
#include <sys/types.h>
void main( )
{	pid_t pid,miopid;
	
	pid=fork( );
	if (pid==0)
		{miopid=getpid( );
		 printf("sono il processo figlio con pid: %i\n\n",miopid);
		 exit( );
		 }
	else 	{printf("sono il processo padre\n");
		 printf("ho creato un processo con pid: %i\n", pid);
		 miopid=getpid( );
		 printf("il mio pid e' invece: %i\n\n", miopid);
		 exit( ); /* non necessaria */
		 }
}
/***************************************************************************/
```

pid_t wait (int *)
	- Sospende l�esecuzione del processo che la esegue e attende la
	terminazione di un �qualsiasi- processo figlio;
	- se un figlio � gi� terminato la wait del padre si sblocca 
	immediatamente (nessun effetto)
	- ritorna il pid del processo figlio terminato
	- Esempio:
		pid_t pid;
		int stato;
		pid = wait (&stato);
	- stato, parametro passato per indirizzo:
		codice di terminazione del processo;
		8 bit superiori: possono essere assegnati esplicitamente 
		come parametro di exit;
		altri bit di stato assegnati dal S.O. per indicare condizioni 
		di terminazione (e.g., errore).
	- exit con parametro
	- void exit (int stato)
	- Esempio: exit(5) 
		termina il processo e restituisce il valore 5 al padre;
		se il padre � gi� terminato lo stato viene restituito
		all�interprete comandi;
		dettaglio importante:
			il valore restituito � costituito dagli 8 bit superiori
			di stato --> lo stato ricevuto da wait � il parametro 
			di exit moltiplicato per 256


```c
/***************************** CODICE **************************************/
#include <stdio.h>
#include <sys/types.h>
void main( )
{	pid_t pid, miopid;
	int stato_exit, stato_wait;
	
	pid=fork( );
	if (pid==0)
		{ miopid=getpid( );
		  printf("sono il processo figlio con pid %i \n", miopid);
		  printf("termino \n\n");
		  stato_exit=5;
		  exit(stato_exit);
		}
	else
		{ printf("ho creato un processo figlio \n\n");
		  pid=wait (&stato_wait);
		  printf("terminato il processo figlio \n");
		  printf("il pid del figlio e' %i, lo stato e' %i\n",pid,stato_wait/256);
		}
}
/***************************************************************************/
```


pid_t waitpid (pid_t pid, int * stato, int opzioni)
	- Esempio: waitpid (10, &stato, opzioni)
	- Mette un processo in stato di attesa dell�evento di terminazione 
	di un processo figlio con pid �pid� (10 in questo caso) e ne 
	restituisce il pid (10 in questo caso); la variabile �stato� assume 
	il valore di �exit� del processo figlio terminato. 
	Il parametro �opzioni� specializza la funzione �waitpid�.


exec
	- sostituisce i segmenti codice e dati 
	(utente; non il segmento di sistema! i file aperti rimangono aperti) 
	del processo in esecuzione con codice e dati di un programma contenuto
	in un file eseguibile specificato;
	- il processo rimane lo stesso (stesso pid): programma != processo!
	- pu� passare parametri al nuovo programma
	(main � una particolare funzione: main (!!))
	- esistono diverse varianti di exec


exec1 (char *nome_programma, char *arg0, char *arg1, �, NULL);
	- nome_programma: stringa che identifica completamente (pathname) 
	il file eseguibile contenente il programma da lanciare
	- arg0, arg1, �: puntatori a stringhe da passare come parametri al 
	main da lanciare; l�ultimo � NULL perch� il numero di arg � variabile.
	- infatti: 
		Il main, che � una particolare funzione, pu� avere a sua 
		volta dei parametri! 


void main (int argc, char *argv[])
	- argc: numero di parametri ricevuti
	- argv[]: vettore di puntatori  a stringhe; 
		ogni stringa � un parametro
		argv[0] contiene sempre il nome del programma


exec1 provoca quindi l�esecuzione del (�chiama� il) programma il 
cui eseguibile si trova nel file nome_programma e gli passa come 
parametri (per indirizzo: sono puntatori) arg0, arg1, �)


```c
/***************************** CODICE **************************************/
#include <stdio.h>
void main (int argc, char *argv[ ] )
{	int i;
	
	printf("\nsono il programma main1\n");
	printf("ho ricevuto %i parametri\n", argc);
	
	for (i=0; i<argc; i++)
		printf("il parametro %i �: %s\n", i, argv[i]);
		
}
/***************************************************************************/
```
```c
/***************************** CODICE **************************************/
#include <stdio.h>
#include <sys/types.h>
void main( )
{	
	char P0[ ]="main1";
	char P1[ ]="parametro 1";	
	char P2[ ]="parametro 2"; 
	
	printf("sono il programma exec1\n");
	
	exec1("/home/pelagatt/esempi/main1", P0, P1, P2, NULL);
	
	printf("errore di exec"); 	/*normalmente non si arriva qui!*/
}
/***************************************************************************/
```

Altre versioni di exec
	- execv: sostituisce alla lista di stringhe di exec1 un puntatore
	a un vettore di stringhe char ** argv 
	- execlp e execvp permettono di sostituire il pathname completo 
	con il solo nome del file nel direttorio di default
	- execle e execve hanno un parametro in pi� che specifica l�ambiente 
	di esecuzione del processo.

```c
/***************************** CODICE **************************************/
#include <stdio.h>
#include <sys/types.h>
void main( )
{	pid_t  pid;
	int stato_wait;
	char P0[ ]="main1";
	char P1[ ]="parametro 1";	
	char P2[ ]="parametro 2"; 
	
	pid=fork( );
	
	if (pid==0)
	{ 	printf("\nsono il processo figlio \n");
		printf("lancio in esecuzione il programma main1\n");
		exec1("/home/pelagatt/esempi/main1", P0, P1, P2, NULL);
		printf("errore di exec"); /*normalmente non si arriva qui!*/
		exit( );
	}
	else
	{	
		wait(&stato_wait );
		printf("\nsono il processo padre\n");
		printf("il processo figlio � terminato\n");
		exit( );
	}
}
/***************************************************************************/
```

```c
/***************************** CODICE **************************************/
main()
{
	int i, j, k, stato;
	pid_t pid1, pid2;
	i=10; j=20; k=30;
	pid1 = fork(); 			/*creazione del primo figlio /
	if (pid1 == 0)  {
			j=j+1;
			pid2 = fork(); /*creazione del secondo figlio */
			if (pid2 == 0) {
					k=k+1;
					exit();}
			else {
					wait(&stato);
					exit();   }
		          }			
	else 	{
		i=i+1;
		wait(&stato);
		exit();   }
}
/***************************************************************************/
```