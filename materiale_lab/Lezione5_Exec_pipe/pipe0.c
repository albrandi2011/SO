#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#define N_MESSAGGI 10
int main()
{int pid, j,k, piped[2];
/*Apre la pipe creando due file descriptor, uno per la lettura e l'altro
per la scrittura, vengono memorizzati nei due elementi dell'array piped*/
if (pipe(piped) < 0) exit(1);
if ((pid = fork()) < 0) exit(2);
else
if (pid == 0)  /* Il figlio eredita una copia di piped[] */
  { /* Il figlio e' il lettore dalla pipe: piped[1] non gli serve */
  close(piped[1]);
  for (j = 1; j <= N_MESSAGGI; j++)
  {read(piped[0],&k, sizeof (int));
    printf("Figlio: ho letto dalla pipe il numero %d\n", k);}
    exit(0);}
else/* Processo padre */
  {/* Il padre e' scrittore sulla pipe: piped[0] non gli serve */
  close(piped[0]);
for (j = 1; j <= N_MESSAGGI; j++)
    {write(piped[1], &j, sizeof (int));}
  wait(NULL);exit(0);}
}