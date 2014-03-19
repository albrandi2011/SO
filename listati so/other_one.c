/*
 * per compilare: gcc other_one.c -Wall -O3 -o other_one
 * per eseguire: ./other_one
 * cosa fa: utilizzo di file per controllare se esiste gia'
 * un'istanza del programma e crea un file temporaneo
 * come funziona: prova a creare un file fittizio "dummy" se il file
 * non esiste. Nel caso il file esita, e' stato creato da un altra
 * instanza del programma. Se e' l'unica instanza attiva del programma
 * genera un file temporaneo e scrive qualcosa in esso (usare
 * "lsof | grep <pid>" per conoscere il nome del file, per vedere il suo
 * contenuto invece "less /proc/<pid>/fd/<file descriptor restituito
 * da lsof>" e notare come il contenuto sia aggiornato dopo fflush).
 * La funzione atexit aggiunge una funzione da eseguire al termine
 * del programma che rimuove il file "dummy", per consentire ad una
 * nuova instanza di venir eseguita. La funzione atexit non e' eseguita
 * in caso di terminazione del processo tramite segnali, quindi in caso
 * di terminazione del processo con SIGINT (ctrl+c) o con
 * SIGTERM, contrariamente all'altro codice di esempio "only_one.c"
 * non viene eseguito un controllo per vedere se e' necessario
 * cancellare il file "dummy" creato, e quindi all'esecuzione di una
 * nuova instanza, questa credera' che non sia l'unica in esecuzione
 * anche in caso contrario.
 * funzioni di rilievo da vedere nel man:
 * - file stream/descrittore file (tmpfile, open, close, fflush, fileno,
 *   fsync, unlink)
 * - processi (getpid)
 * - segnali (signal, sigemptyset, sigaddset, sigprocmask)
 * - varie (TEMP_FAILURE_RETRY, errno).
 */

#include "other_one.h"

// funzioni da eseguire al termine
void make_clean(void) {

    printf("\nDo exit\nCleaning file\n");
    unlink("dummy");
}

int main(int argc, char *argv[]) {
    FILE *tmp_file;
    int the_file, t, p;
    char only_file[]="dummy";

    // ottieni il pid
    p=getpid();
    printf("Pid: %d\n",p);

    // crea il file se non esiste utilizzando le primitive di apertura
    // a file. Poiche' nel caso di interruzione da segnale segnalata con
    // errno=EINTR la chiamata a open non va a buon fine utilizza la
    // macro TEMP_FAILURE_RETRY definita nell'header che prova a
    // richiamare la funzione finche' la variabile errno non e' diversa
    // da EINTR. I flag fanno in modo che il file venga creato e la
    // funzione dia errore se il file gia' esisteva
    the_file=TEMP_FAILURE_RETRY(open(only_file,O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR));
    // controlla se il file esisteva
    t=(the_file==-1)&&(errno==EEXIST);

    // nel caso la creazione di "dummy" e' andata a buon fine e quindi
    // non ci sono altre instanze del programma attive
    if (!t) {
        // aggiungi la funzione di pulizia alla chiusura del programma
        // nota: non viene eseguita quando si termina su arrivo di
        // segnale
        atexit(make_clean);

        printf("Only one last\n");

        // crea un file temporaneo
        tmp_file=tmpfile();
        if (!tmp_file)
            printf("Error\n");
        printf("Temp file created\n");

        // scrivi qualcosa in esso
        fprintf(tmp_file,"ADSSODJLKNALK\n");

        // poiche' fflush non e' esguito il suo contenuto e' vuoto
        printf("Check out the temp file content\n");
        getchar();

        // esegui fflush
        fflush(tmp_file);
        // ora il file contiene la stringa di caratteri.
        // Attenzione, e' stata solo aggiornata la cache file nel
        // kernel. E' possibile nel caso di file fisici <(FILE *)file>
        // su disco che non siano ancora scritti effettivamente,
        // utilizzare fsync(fileno(<file>)) in questo caso
        printf("Check out the temp file content again\n");
        getchar();

    } else
        // c'era qualcuno
        printf("There is somebody...\n");

    exit(EXIT_SUCCESS);
}
