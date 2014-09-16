#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define NPHILS 5
#define ITER 3
char *progname;
/* the chopsticks */
pthread_mutex_t chopstick[NPHILS];
/* print error message and die */
void error(char *f)
 { extern char *progname;
 if (progname) fprintf(stderr, "%s: ", progname);
 perror(f);exit(1);
}
 /* suspend execution of the calling thread */
void waiting(int min, int max)
 {sleep(rand()%(max-min+1) + min); }
void rightChopstick(int id)
{if (pthread_mutex_lock(&chopstick[id]))
  error("pthread_mutex_lock");
 printf("#%d got right chopstick\n", id);
}
void leftChopstick(int id)
{if(pthread_mutex_lock(&chopstick[(id+1) % NPHILS]))
  error("pthread_mutex_lock");
 printf("#%d got left chopstick\n", id);
}
void *philosopherRoutine(void *idp)
{ int id=*(int *)idp, i;
  for (i=0; i<ITER; i++)
  { printf("#%d is thinking\n", id);
   waiting(1, 10);
   printf("#%d is hungry\n", id);
   if (id % 2)
   {rightChopstick(id);
     waiting(1,2);
     leftChopstick(id);
    }
   else
    {leftChopstick(id);
     waiting(1,2);
     rightChopstick(id);
     }
   printf("#%d is eating\n", id);
   waiting(1, 10);
   if (pthread_mutex_unlock(&chopstick[id]))
     error("pthread_mutex_unlock");
   printf("#%d left right chopstick\n", id);
   if (pthread_mutex_unlock(&chopstick[(id+1) % NPHILS]))
    error("pthread_mutex_unlock");
   printf("#%d left left chopstick\n", id);
  }
 return NULL;
}
int main(int argc, char *argv[])
{int i;
  struct { int id;pthread_t thread_id;}
    philosopher[NPHILS];
  progname=argv[0];
  srand(time(NULL));
  /* create mutex semaphores */
  for (i=0; i<NPHILS; i++)
   if (pthread_mutex_init(&chopstick[i], NULL))
    error("pthread_mutex_init");
  /* create and run the threads */
  for (i=0; i<NPHILS; i++)
   {philosopher[i].id=i;
     if (pthread_create(&philosopher[i].thread_id, NULL,
      philosopherRoutine, &philosopher[i].id))
      error("pthread_create");
   }
  /* wait for the threads to terminate */
for (i=0; i<NPHILS; i++)
 if (pthread_join(philosopher[i].thread_id, NULL))
  error("pthread_join");
 return 0;
}

