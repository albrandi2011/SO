#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

void *do_something_write(long int who_i_am);
void *do_something_read(long int who_i_am);

void sig_user_exit(int signum);

int main(int argc, char *argv[]);
