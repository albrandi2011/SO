#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<time.h>


typedef struct{
   pid_t pid;
   int id;
   int occorrenze;
}processo;
int main(int argc, char *argv[])
{  int **matr, n, m, j, i, w, k;
   processo *figli;

   if(argc!=4){
     printf("Errore!!!");
   exit(EXIT_FAILURE);
  }

 k=atoi(argv[1]); //intero
 n=atoi(argv[2]); //righe
 m=atoi(argv[3]); //colonne
    if((n<1)||(n>10)||(m<10)||(m>255)){
       printf("NON CONSENTITO!!\n");
      exit(-1);
     }
  figli=(processo *)malloc(n*sizeof(processo));

      matr=(int **)malloc(n*sizeof(int *));
        for(i=0; i<n; i++)
         matr[i]=(int *)malloc(m*(sizeof(int)));

    for (i=0; i<n; i++) {              //inizializzo la matrice e la stampo
        printf("\n");
        for (j=0; j<m; j++) {
             matr[i][j] = rand() % 10;
             printf("%d   ",matr[i][j]); 
        }     
    }
    printf("\n\n\n");
   
   for(i=0; i<n; i++){
     figli[i].id=i;
      figli[i].occorrenze=0;
      figli[i].pid=fork();
       if(figli[i].pid==0)   
     exit(i);
    }

      for(i=0; i<n; i++){
        for(j=0; j<m; j++){
             if(matr[i][j]==k)
            figli[i].occorrenze++;
             }
          }
    sleep(1);
    for (i=0;i<n;i++){
          wait(&w);
        if(WIFEXITED(w))
       printf("occorrenze di %d nella riga %d è: %d\n", k, i, figli[i].occorrenze);
  }
 exit(EXIT_SUCCESS);
}
