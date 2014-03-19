#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#define N 10

int i=1;
void legge(int file)
{FILE *fstream;
  int c;
    fstream=fdopen(file,"r");
        while ((c=fgetc(fstream))!=EOF)
       putchar(c);
  fclose(fstream);
}
void chiede_turno(int file)
{FILE *fstream;

   fstream=fdopen(file,"w");
// scrive qualcosa nella pipe
        fprintf(fstream,"Mi dai il turno??\n");
fflush(fstream);
      fclose(fstream); 
}
//comunicazione del turno
void comunico_turno(int file)
{FILE *fstream;

   fstream=fdopen(file,"w");
// scrive qualcosa nella pipe
        fprintf(fstream,"Adesso è il tuo turno %d\n",i);
fflush(fstream);
      fclose(fstream); 
}
void finito(int file)
{FILE *fstream;

   fstream=fdopen(file,"w");
        fprintf(fstream,"Ho finito\n");
fflush(fstream);
      fclose(fstream); 
}
int main(int argc, char *argv[]){
 int w, tube[2],piped[2],fine[2];
 pid_t padre, ok_fork;

  if(argc!=1){
   printf("Errore!!\n");
    exit(-1);
   }
   if (pipe(piped)) //serve per riconoscere la pipe
     {printf("Error!!!File %s, line %d\n",__FILE__,__LINE__); exit(EXIT_FAILURE);}
    if (pipe(tube))
     {printf("Error!!!File %s, line %d\n",__FILE__,__LINE__); exit(EXIT_FAILURE);}
   if (pipe(fine))
     {printf("Error!!!File %s, line %d\n",__FILE__,__LINE__); exit(EXIT_FAILURE);}
   padre=getpid();
        if (padre)
         if ((ok_fork=fork())<0)
          {printf("Error!!File %s,line %d\n",__FILE__,__LINE__);exit(EXIT_FAILURE);}
     
         for(i=0; i<N; i++){
           if (ok_fork==0){
           close(tube[0]); //il figlio chiude il canale lettura (deve solo scrivere)
             chiede_turno(tube[1]); // e scrive nella pipe
              close(piped[1]);
             legge(piped[0]);
              srand(time(NULL));
               sleep(rand()%5);
              close(fine[0]);
             finito(fine[1]);}
          if(padre){
          close (tube[1]); //il padre chiude il canale scrittura(deve solo leggere)
            legge(tube[0]);
            close(piped[0]);
           comunico_turno(piped[1]);
            close(fine[1]);
            legge(fine[0]); 
           }
        }
         
    wait(&w);
exit(EXIT_SUCCESS);
}
