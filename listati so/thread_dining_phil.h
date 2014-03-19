#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#define NPHIL 5
#define NSEC 10

#define RIGHT(i) i
#define LEFT(i) ((i==0)?NPHIL-1:i-1)

void sig_user(int signum);
void phil(int n);
int main(int argc, char *argv[]);
