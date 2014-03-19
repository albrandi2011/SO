#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>



int main()
{
 int pid,i=5,j;
 int piped[2];

 if(pipe(piped)<0)
  exit(1);

 if ((pid=fork())<0)
 {
  printf("Fork fallita..\n");
  exit(2);
 }
 else if(pid==0)
 {
  close(piped[1]);
  
  read(piped[0],&j,sizeof(int));
  printf("Il figlio ha letto dalla pipe il numero %d\n",j);
 }
 else
 {
   close(piped[0]);

   write(piped[1],&i,sizeof(int));
   printf("Il padre scrive nella pipe il numero %d\n",i);
 }

}
