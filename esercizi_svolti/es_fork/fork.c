#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>


int main(int argc, char *argv[])
{  pid_t pid;

   pid=fork();
  
   if(pid<0){
    printf("ERRORE!!\n");
    exit(EXIT_FAILURE);}
   if(pid==0){
     printf("Sono il figlio %d\n", getpid());}
    else
     printf("Sono il padre %d\n", getpid());
}
