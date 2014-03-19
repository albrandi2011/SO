#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main(int argc, char *argv[]){
 int n, w, k, x;
 char *nome;
 pid_t padre, figlio1, figlio2;
 FILE *fp;

    if(argc!=3){
         printf("Si è verificato un'errore\n");
      exit(-1);
      }
 n=atoi(argv[1]);
 nome=argv[2];

    padre=getpid();
    
     if((figlio1=fork())<0){
         printf("Errore....\n");
         exit(-1);}
      if(figlio1==0){
        sleep(n);
       fp=fopen(nome,"w");
        fprintf(fp,"%d",n);
       fclose(fp);
     exit(1);
      }
    if(padre)
        wait(&w);
  
    if((figlio2=fork())<0){
         printf("Errore....\n");
         exit(-1);}
      if(figlio2==0){
       fp=fopen(nome,"r");
       fscanf(fp,"%d",&x);
        printf("%d\n", x);
       fclose(fp);
     exit(2);
      }
        wait(&k);

exit(EXIT_SUCCESS);
}
