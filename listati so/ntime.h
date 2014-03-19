#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/times.h>
#include <unistd.h>
#include <math.h>

typedef struct tictac {
	clock_t t;
	struct tms c;
} tictac;

typedef struct stime {
	// tempo effettivo (totale)
	double realt;
	// tempo di esecuzione delle istruzioni proprie del processo
	// (i.e. nello spazio utente)
	double usert;
	// tempo di esecuzione delle chiamata di sistema (i.e. nel kernel)
	double syst;
} stime;

#define SILENT 	0
#define VERBOSE 1

//double trunc(double x);
int main(int argc, char *argv[]);
tictac gimme_clock(void);
stime elapsed_time(tictac start, tictac stop, int mode);
