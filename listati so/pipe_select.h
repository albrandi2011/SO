#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>
#include <sys/times.h>

typedef struct tictac {
	clock_t t;
	struct tms c;
} tictac;

void put_into_tube(int file);
void look_into_tube(int *file);
void sig_cont(int signum);
void sig_dummy(int signum);

int main(int argc, char *argv[]);


