#include<stdio.h>
#include <unistd.h>
#include<signal.h>
#include <stdlib.h>
void announce();
int main (int argc, char *argv[]) 
 {if(argc!=2)
   {printf("Uso: %s secondi\n",argv[0]);exit(1);}
  signal(SIGALRM,announce);
  alarm((unsigned)atoi(argv[1]));
  pause();} /* attende un segnale */
void announce() 
{ fprintf(stdout,"Sveglia! \n");
  exit (0);}