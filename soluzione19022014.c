/**
 * Scrivere un programma multi processo in linguaggio C:
 *
 * 1. un processo padre che sia responsabile della generazione di una matrice
 * M(m,n) di interi a 16bit casuali, inoltre le dimensioni saranno date in input e la dimensione m della matrice deve essere pari.
 *
 * 2. Due processi figli che abbiano il compito di calcolare un vettore somma.
 * Ciascun elemento del vettore di ciascun figlio conterrà la somma di tutti
 * i valori della riga della matrice.
 * Infine i due vettori saranno restituiti al padre per il calcolo delle
 * relative somme e la relativa visualizzazione sul monitor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#define CHILDREN 2

void stampa_matrice(short int **M, int m, int n)
{
	int i, j;

	puts(" -- -- -- STAMPO MATRICE -- -- --");
	for(i=0; i < m; i++)
	{
		for(j=0; j < n; j++)
			printf("%d ",M[i][j]);
		putchar('\n');
	}
	puts(" -- -- -- FINE STAMPO MATRICE -- -- --");
}

int main(int argc, char **argv)
{
	//Matrice
	short int **M,
						//Vettore1
						*F1_Vet,
						//Vettore 2
						*F2_Vet;
	//Righe e colonne
	int m, n,
			//Variabili ciclose
			i, j, y, x,
			//Somma
			somma=0,
			//File Descriptor
			fd[2];

	// Pid
	pid_t pid[2];


	//Se i parametri non coincidono
	if(argc != 3 || (atoi(argv[1]) % 2))
	{
		printf("usage: %s <m> <n>\n<m> - Riga Pari\n<n> Colonna\n", argv[0]);
		return -1;
	}

	//Prendo i parametri
	m = atoi(argv[1]); //Righe
	n = atoi(argv[2]); //Colonne

	//Inizializzo il segno
	srand(time(NULL));

	M = (short int**) malloc(m * sizeof(short int*));

	//Alloco e popolo la matrice
	for(i=0; i < m; i++)
	{
		M[i] = (short int*) malloc(n * sizeof(short int));
		for(j=0; j < n; j++)
			M[i][j] = (short int) rand() % 100+1;
	}

	//Creo la pipe
	if(pipe(fd) < 0)
	{
		perror("Errore nella creazione della pipe\n");
		return -3;
	}

	// 8==> ~ (Y)
	for(i=0; i < 2; i++)
	{
		//Conservo il pid dei miei figli.
		switch(pid[i] = fork())
		{
			case -1:
				puts("Errore nella Pipe!");
				exit(-1);
			case 0:
				// Sono il primo figlio
				if(i == 0)
				{
					//Array1
					F1_Vet = (short int*) calloc(m, sizeof(short int));
					printf("Sono il primo figlio, calcolo le righe pari\n");
					//Calcolo le righe pari
					for(j=1; j <= m; j+=2)
						for(y=0; y < n; y++)
							F1_Vet[j-1] += M[j][y];
					//Figlio di che padre ormai. Libero camminerai.
					close(fd[0]);
					write(fd[1], F1_Vet, sizeof(F1_Vet));
					puts("Addio, mondo crudele!");
					exit(1);
				}
				else
				{
					//Array2
					F2_Vet = (short int*) calloc(m, sizeof(short int));
					printf("Sono il secondo figlio, calcolo le righe dispari\n");
					//Calcolo le righe dispari
					for(j=0,x=0; j < m; j+=2, x++)
						for(y=0; y < n; y++)
							F2_Vet[x] += M[j][y];
					//Ora lo dico a papà
					close(fd[0]);
					write(fd[1], F2_Vet, sizeof(F2_Vet));
					puts("Addio, mondo crudele!");
					exit(1);
				}
				break;
			default:
				//Aspetto quei disgraziati.
				wait(NULL);
		}
	}
	//Padre
	puts("Addio, figli miei :(");

	F1_Vet = (short int*)malloc(m * sizeof(short int));
	F2_Vet = (short int*)malloc(m * sizeof(short int));

	// I figli hanno lasciato un pizzino
	close(fd[1]);
	read(fd[0], F1_Vet, sizeof(F1_Vet));
	read(fd[0], F2_Vet, sizeof(F2_Vet));

	// Stampo la matrice creata in precedenza
	stampa_matrice(M, m, n);

	// Sommo i valori passati dai miei figli
	for(i=0; i < m; i++)
		somma += F1_Vet[i] + F2_Vet[i];

	printf("RISULTATO FINALE: %d\n", somma);

	return 0;
}
