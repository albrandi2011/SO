#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>


int main()
{
 int pid[2],i;
 int pipe1[2],pipe2[2],pipe3[2];
 int mex[2];
 int n1=5,n2=8;

 srand(time(NULL));

 if (pipe(pipe1)<0)
   exit(1);
 if (pipe(pipe2)<0)
   exit(1);
 if (pipe(pipe3)<0)
   exit(1);
 
 for(i=0;i<2;i++)
 {
  if ((pid[i]=fork())<0)
  {
    printf("Fork fallita...\n");
    exit(2);
  }
  else if (pid[i]==0)
  {
    
    printf("Sono il figlio %d comunico a mio padre il numero %d\n",i,n1);
    exit(0);
  }
  else
  {
    sleep(1);
   
    printf("Sono il padre leggo dalla pipe e comunico al figlio in numero \n");
  }
sleep(1);
 } 

}
