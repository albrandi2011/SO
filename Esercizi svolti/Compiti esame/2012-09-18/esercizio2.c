#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#include<signal.h>
#define CANOA 0
#define PEDALO 1

typedef struct{
  pid_t pid;
  int id;
  int peso;
  int scelta;
}str_processo;

int main(int argc, char *argv[]){
 int N, i, canoa=0, w, somma=0, j;
 pid_t pid;
  str_processo *natanti;
  if(argc!=2){
    printf("Errore!!");
     exit(-1);
    }
 N=atoi(argv[1]);
   if(N<9){
     printf("NON consentito!!");
    exit(-1);
   }
   natanti=(str_processo *)malloc(N*sizeof(str_processo));
    srand(time(NULL));
  
    for(i=0; i<N; i++){
     natanti[i].id=i;
     natanti[i].peso=(rand()%91)+50;
     natanti[i].scelta=rand()%2;
      }
       for(i=0; i<3; i++){
        pid=fork();
        if(pid==0){
           for(j=0; j<N; j++){
           if(natanti[j].scelta==0){
              canoa++;
                if(canoa==2)
                  pause();                  
               if(canoa<2){
                  raise(SIGCONT);
                 printf("sto facendo il giro in canoa!\n");
                 sleep(rand()%5);
                  canoa--;
                 }
               }
          if(natanti[j].scelta==1){
             somma=natanti[j].peso+somma;
               if(somma>=500){
                printf("Facciamo un giro in pedalo!\n");
                  sleep(rand()%5);
                  somma=0;
                   }
                   }
          }
       exit(i);
     }
     }
  for(i=0; i<3; i++){
    wait(&w);
   }
  printf("NA	PESO	SCELTA\n");
    for(i=0; i<N; i++){
    printf("%d	   %d	  %d\n",i, natanti[i].peso, natanti[i].scelta);}
exit(EXIT_SUCCESS);
}
