#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#ifndef TEMP_FAILURE_RETRY
  #define TEMP_FAILURE_RETRY(expr) \
    ({ long int _res; \
       do _res = (long int) (expr); \
       while (_res == -1L && errno == EINTR); \
       _res; })
  #endif

void sig_user(int signum);
int main(int argc, char *argv[]);


