#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
int count = 0;
void gotCntrlBSlash();
void gotINT();
int main (int argc, char* argv[]) 
 {int i;
  signal(SIGTSTP,SIG_IGN); // ignora ^Z
  signal(SIGQUIT,gotCntrlBSlash);// gestore per '^\'
  signal(SIGINT,gotINT); // gestore per ^C
  for(i=1;i<100;i++) {sleep(1);printf((i%10==0)?".\n":".");}
  return 0;
  }
void gotINT()
 {switch(++count)
   {case 1:printf("Ricevuto Primo SIGINT\n");
           signal(SIGINT,gotINT);break;
    case 2:printf("Ricevuto Secondo SIGINT\n");
           signal(SIGINT,gotINT);break;
    case 3:printf("Ricevuto Terzo SIGINT\n");
           signal(SIGINT,SIG_DFL);break;}}
void gotCntrlBSlash()
  {signal(SIGQUIT,gotCntrlBSlash);
    printf("Ricevuto un SIGQUIT\n");
}