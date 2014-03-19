#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

#define NTHREADS 5
#define SEM_RES 3

void *do_something(long int who_i_am);
void sig_user_exit(int signum);
void set_sem(void);

int main(int argc, char *argv[]);
