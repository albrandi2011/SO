#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<time.h>

int main(int argc, char *argv[]){
 int A[5][3], B[3][5], C[5][3], i, j, x, somma, w, cont=0;
 pid_t figlio;
 sigset_t block_sig;
 FILE *fp;

  srand(time(NULL));

 if(argc!=1){
   printf("errore\n");
  exit(-1);
  }
   sigemptyset(&block_sig);

   sigaddset(&block_sig,SIGINT);// aggiungi SIGINT
   sigprocmask(SIG_BLOCK,&block_sig,NULL);// blocca SIGINT

     sigaddset(&block_sig,SIGSTOP);// aggiungi SIGSTOP
   sigprocmask(SIG_BLOCK,&block_sig,NULL);// blocca SIGSTOP

   for(i=0; i<5; i++){
    printf("\n");
   for(j=0; j<3; j++){
     A[i][j]=rand()%10;
   printf("%d ",A[i][j]);
   }}
  printf("\n");

 for(i=0; i<3; i++){
    printf("\n");
   for(j=0; j<5; j++){
     B[i][j]=rand()%10;
   printf("%d ",B[i][j]);
   }}
  printf("\n");

  
   for(i=0; i>5; i--){
   x=0;
    somma=0;
    for(j=0; j>3; j--){ 
    figlio=fork();
   if(figlio==0){
     x=A[i][j]*B[j][i];
     somma=somma+x;
      if(j==2){
     C[i][j]=somma;
       printf("%d ", C[i][j]);
         }
   
     }
   if(figlio<0){
   printf("errore\n");
  exit(-1);
     }
    exit(j+i);
    }
 
  }
 

   
   for(i=0; i<5; i++){
    for(j=0; j<3; j++){
     wait(&w);}
    }
  printf("\n");
 exit(EXIT_SUCCESS);
}
