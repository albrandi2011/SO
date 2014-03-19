#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define PROCESSI 10
typedef struct {
        pid_t pid;
        int id;
        int occorrenze;
}str_figlio;
char *stringa, *x;
void main(int argc, char *argv[])
{
     int i, j, n, max=0, proces;
     str_figlio *figlio;
     pid_t ok_fork;

     if (argc != 2) {
        printf("inserisci il parametro in modo corretto\n");
        exit(-1);
     }

     stringa = argv[1];
     n = strlen(stringa);
      figlio=(str_figlio *)malloc(PROCESSI*sizeof(str_figlio));

     for (i=0;i<PROCESSI;i++) {
         ok_fork = fork();
         if (ok_fork< 0) {
             printf("si è verificato un errore!\n");
             exit(EXIT_FAILURE);
         }     
         else {
             if (ok_fork == 0) {
              figlio[i].pid=ok_fork;
              figlio[i].id = i;
               figlio[i].occorrenze = 0;
                sprintf(&x,"%d",figlio[i].id);
                for (j=0; j<n; j++) {
                   
                    if (x == stringa[j])
                       figlio[i].occorrenze++;
                }
                printf("Le occorrenze di %d sono: %d\n",i,figlio[i].occorrenze);
              exit(i);
             }
         }
     }
     sleep(1);
    for (i=0;i<10;i++) {
          waitpid(figlio[i].pid,&figlio[i].id,0);
          if (WIFEXITED(figlio[i].id)) {
              figli[i].occorrenze = WEXITSTATUS(figlio[i].id);
              if (max< figlio[i].occorrenze){
                  max= figlio[i].occorrenze;                     
              }
          }
     }
     printf("La maggiore occorrenza è: %d\n", max);
   exit(EXIT_SUCCESS);

}
