/*
 * per compilare: gcc fifo.c -Wall -O3 -o fifo
 * per eseguire: ./fifo [read | write]
 * cosa fa: utilizzo di una fifo tra due processi differenti nella
 * stessa cartella per scambio di dati.
 * come funziona: il programma prova a generare la fifo nel proprio path
 * e attende in lettura o scrittura in base a come viene chiamato (i.e.
 * lettura/scrittura). Una seconda instanza del programma viene chiamata
 * per (scrivere/leggere) e successivamente avviene lo scambio dati. E'
 * possibile notare con un semplice comando "ls" la fifo creata prima
 * di lanciare la seconda instanza del programma. La variabile esterna
 * errno e' impostanza di conseguenza se la fifo era gia' stata creata.
 * funzioni di rilievo da vedere nel man:
 * - fifo (mkfifo, remove)
 * - generatore pseudorandom (rand, rands)
 * - file stream/descrittore file (fopen, fclose, fflush)
 * - processi (getpid)
 * - varie (getcwd, argc, argv, errno).
 */

#include "fifo.h"

// legge dalla fifo
void from_fifo(char *file) {
    int c;
    FILE *fstream;

    // apre la fifo in lettura
    fstream=fopen(file,"r");
    // finche' qualcuno ha aperto la fifo per scrivere leggi il
    // contenuto
    while ((c=fgetc(fstream))!=EOF)
        // e mostralo sullo schermo
        putchar(c);
    // chiudi la fifo
    fclose(fstream);
}

// scrive nella fifo
void to_fifo(char *file) {
    int i;
    FILE *fstream;

    // apri la fifo in scrittura
    fstream=fopen(file,"w");
    // stampa header
    fprintf(fstream,"something:\n");
    // stampa dei caratteri random
    for (i=0;i<26;i++)
        fputc('a'+(char)((rand()+getpid())%26),fstream);
    fprintf(fstream,"\n");
    // chiudi la fifo
    fclose(fstream);
    printf("Data sent\n");
}

int main(int argc, char *argv[]) {

    int d=0;
    int fifo;
    char *cwd, cwd_[PATH_MAX];
    char *fifo_name=FIFO_NAME;
    char *full_fifo_name;
    int i, l;

    // se il programma non e' stato chiamato correttamente termina
    if (argc!=2) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // controlla se e' stato chiamato per leggere
    if (!strcmp("read",argv[1]))
        d=-1;
    // o scrivere nella fifo
    else if (!strcmp("write",argv[1]))
        d=1;
    // oppure esci
    else  {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }

    // ottieni il percorso corrente
    cwd=getcwd(cwd_,PATH_MAX);
    // se il percorso e' maggiore del buffer allocato segnala l'errore
    if ((!cwd) && (errno==ENAMETOOLONG)) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }
    /*
     * per evitare controlli e' possibile sostituire la riga di codice
     * con:
     * cwd=get_current_dir_name();
     * e ricordarsi alla fine di liberare la memoria con
     * free(cwd);
     */
    // e genera il nome della fifo tenendo conto della lunghezza del
    // nome
    l=strlen(cwd)+2+strlen(fifo_name);
    // e dell'allocazione dello spazio necessario
    full_fifo_name=(char *)malloc(sizeof(char)*l);
    // copia la descrizione del percorso
    for (i=0;cwd[i]!='\0';i++)
        full_fifo_name[i]=cwd[i];
    // aggiungi il carattere "/"
    full_fifo_name[i++]='/';
    // e il nome da dare alla fifo
    for (l=0;fifo_name[l]!='\0';full_fifo_name[i++]=fifo_name[l++]);
    full_fifo_name[i]='\0';

    // prova a generare la fifo con i permessi di scrittura e lettura
    if ((fifo=mkfifo(full_fifo_name,S_IRUSR|S_IWUSR))) {
        // se esiste continua (la prima instanza del programma e' gia'
        // in esecuzione)
        if (errno==EEXIST)
            printf("FIFO already created\n");
        // termina in caso di errore
        else {
            printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }
    }

    // avvisa se e' stata gia' creata la fifo
    if (!fifo)
        printf("FIFO created\nWaiting...\n");

    // leggi se devi
    if (d==-1)
        from_fifo(full_fifo_name);

    // scrivi altrimenti
    if (d==1)
        to_fifo(full_fifo_name);

    // elimina la fifo
    remove(full_fifo_name);
    // disalloca lo spazio utilizzato per memorizzare il nome della fifo
    free(full_fifo_name);

    exit(EXIT_SUCCESS);
}
