#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
int main(int argc, char *argv[])
 { int pid;
  if ((pid = fork()) == 0)
    while (1); // il figlio attende per sempre
  else
   {sleep(10);// il padre attende 10 secondi
    // ? e quindi invia il segnale di terminazione
    kill(pid, SIGINT); 
   }
}