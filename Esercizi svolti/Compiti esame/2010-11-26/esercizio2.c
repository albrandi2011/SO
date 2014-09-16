#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>

char *a, *b;
void sign_print1(int signum){
 char *n;
  printf("Ho ricevuto il segnale SIGUSR1..\n");
   n=strstr(b, a);
    if(n!=NULL)
      printf("La stringa %s è sottostringa di %s\n",a, b);
   else
     printf("La stringa %s NON è sottostringa di %s\n",a, b);
}
void sign_print2(int signum){
  printf("Ho ricevuto il segnale SIGUSR2...\n");
   printf("concatenazione: %s\n",strcat(a,b));
}

int main(int argc, char *argv[]){
  
 void (*sig_old2)(int);
 void (*sig_old1)(int);
   if(argc!=3){
    printf("ERRORE!!");
   exit(EXIT_FAILURE);
   }
  a=argv[1];
  b=argv[2];
   
    sig_old2=signal(SIGUSR2,sign_print2);
    sig_old1=signal(SIGUSR1,sign_print1);

      kill(getpid(),SIGUSR1);
     sleep(2);
     kill(getpid(),SIGUSR2);
      raise(SIGINT);   

  signal(SIGUSR2,sig_old2);
  signal(SIGUSR1,sig_old1);
 exit(EXIT_SUCCESS);
 }

