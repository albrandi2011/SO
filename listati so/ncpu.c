/*
 * per compilare: gcc ncpu.c -Wall -O3 -o ncpu
 * per eseguire: ./ncpu
 * cosa fa: permette di conoscere il numero di cpu presenti
 * come funziona: apre il file di sistema /proc/cpuinfo contenente
 * le informazioni sui processori (provare il comando
 * less /proc/cpuinfo) e conta il numero di volte che e' presente la
 * stringa "processor : <numero di cpu>" tramite espressione regolare
 * (estesa)
 * funzioni di rilievo da vedere nel man:
 * - espressioni regolari (regcomp, regex, regfree)
 * - varie (getline, exit).
 */

#include "ncpu.h"

int main(int argc, char *argv[]) {

    // file stream
    FILE *cpuinfo;
    // buffer per la linea corrente del file
    char *buffer=NULL;
    size_t buffer_length=0;
    // utilizzata dall'espressione regolare
    regex_t pattern;
    // contatore per il numero di cpu
    int ncpu=0;

    // apre il file /proc/cpuinfo in lettura
    if (!(cpuinfo=fopen(CPU_INFO_FILE,"r"))) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // compila l'espressione regolare (estesa) in modo da
    // poterla utilizzare (notare i flag utilizzati)
    if (regcomp(&pattern,CPU_PATTERN,
        REG_EXTENDED|REG_NOSUB|REG_NEWLINE)) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // legge per riga il file /proc/cpuinfo
    while (getline(&buffer,&buffer_length,cpuinfo)>0) {
        // aggiorna il contatore se viene trovata un'instanza
        // dell'espressione regolare (estesa)
        ncpu+=(!regexec(&pattern,buffer,(size_t)0,NULL,0));
    }

    // ripulisce lo spazio utilizzato dall'espressione regolare
    regfree(&pattern);
    // chiude il file
    fclose(cpuinfo);

    // stampa sullo schermo il numero di cpu presenti
    printf("--================--\n"\
           "  Number of CPU: %d\n"\
           "--================--\n",ncpu);

    // esce
    exit(EXIT_SUCCESS);
}
