#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_THREADS 8

typedef struct {
	int arg_int;
	char *arg_string;
} arg_struct;

int *do_struct_arg(arg_struct *in);
int *do_int_arg(int *in);
void free_key(double *val);
void execute_once(void);

int main(int argc, char *argv[]);
