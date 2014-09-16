#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#define N 5

int main(int argc, char *argv[]){
 int i, j=1, w, c;
 pid_t pid, padre, figlio;
 FILE *fp;
  if(argc!=1){
   printf("Errore!!\n");
    exit(-1);
   }
 srand(time(NULL));
   for(i=0; i<4; i++){
      pid=fork();
       if(pid<0){
        printf("Errore!!\n");
      exit(-1);
        }
   padre=getpid();
     if(pid==0){
       figlio=getpid();
      while(j<=N){
       if(figlio){
        fp=fopen("ciao.txt","w");
         fprintf(fp,"che turno e'??\n");
        fclose(fp);}
        if(padre){
        fp=fopen("ciao.txt","a+");
            while ((c=fgetc(fp))!=EOF)
       putchar(c);
         fprintf(fp,"E' il turno %d\n", i);
         fclose(fp);
         }
         if(figlio){
           sleep(rand()%6);
         fp=fopen("ciao.txt","w");
         fprintf(fp,"Ho finito\n");
           fclose(fp);}
        if(padre){
          fp=fopen("ciao.txt","r");
           while((c=fgetc(fp))!=EOF)
            putchar(c);
          fclose(fp);
            j++;}
            }
          exit(i);
          }
    
 for(i=0; i<4;){
    wait(&w);
   if(WIFEXITED(w))
     i++;
  }
exit(EXIT_SUCCESS);
}
