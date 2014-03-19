#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define FIFO_NAME "fifo_tube"
#define PATH_MAX 1024

void to_fifo(char *file);
void from_fifo(char *file);

int main(int argc, char *argv[]);

 
