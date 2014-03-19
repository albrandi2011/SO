#include<stdio.h>
#include<stdlib.h>
#include<time.h>


int main()
{
 int N,i;
 int *numeri;  
 int op,x;
 FILE *fp;
 char buf[200];
 char *res;
  
 
 printf("Digita il numero di interi che vuoi inserire\n");
 scanf("%d",&N);
 
 numeri=(int *)malloc(N*sizeof(int));

 for(i=0;i<N;i++)
 {
  printf("Inserisci\n");
  scanf("%d",&numeri[i]);
 }

 int pid[N-1];
 
 for(i=0;i<N-1;i++)
 {
  srand(time(NULL));
  if((pid[i]=fork())<0)
  {
   printf("Fork fallita...\n");
   exit(1);
  }
  else if (pid[i]==0)
  {
   op=rand()%4;
   if (op==0) //+
   {
    x=numeri[i]+numeri[i+1];
    printf("Ho scritto sul file il risusltato dell'operazione %d + %d\n",numeri[i],numeri[i+1]);
    sleep(1);
   }
   else if (op==1) //-
   {
     x=numeri[i]-numeri[i+1];
     printf("Ho scritto sul file il risusltato dell'operazione %d - %d\n",numeri[i],numeri[i+1]);
     sleep(1);
   }
   else if (op==2) //*
   {
     x=numeri[i]*numeri[i+1];
     printf("Ho scritto sul file il risusltato dell'operazione %d * %d\n",numeri[i],numeri[i+1]);
     sleep(1);
   }
   else // /
   {
     x=numeri[i]/numeri[i+1];
    printf("Ho scritto sul file il risusltato dell'operazione %d / %d\n",numeri[i],numeri[i+1]);
    sleep(1);
   }
    fp=fopen("calc.txt","w");
    fprintf(fp,"%d\n",x);
    fclose(fp);

   exit(0); 
  }
  else
  {
   wait(NULL);
   fp=fopen("calc.txt","r");
    while(1) 
    {
     res=fgets(buf, 200, fp);
     if( res==NULL )
       break;
     printf("Sono il padre ho letto dal file il numero %s", buf);
    }
   fclose(fp);
  }
 }
}
