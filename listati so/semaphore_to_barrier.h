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

#define NPROC 6
#define NSEC 10
#define N 3

#define NO_EINTR(stmt) while ((stmt)==-1 && errno==EINTR);

void sig_user(int signum);
int barrier_from_semaphore(void);
int main(int argc, char *argv[]);


