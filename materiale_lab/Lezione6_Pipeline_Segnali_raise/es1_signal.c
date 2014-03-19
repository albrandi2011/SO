#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
void int_handler(int sig);

void int_handler(int sig)
{printf("Ricevuto!\n");
 exit(2);
}
int main(int argc, char *argv[])
{int pid;
 signal(SIGINT, int_handler);
 if ((pid = fork()) < 0)
  {perror("fork fallita\n");exit(-1); }
 if(pid== 0) 
  {while(1) 
    { sleep(1); printf("In esecuzione!!!\n");}
  }
 sleep(3);
 kill(pid, SIGINT);exit(0);
}