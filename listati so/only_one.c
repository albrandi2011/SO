/*
 * per compilare: gcc only_one.c -Wall -O3 -o only_one
 * per eseguire: ./only_one
 * cosa fa: utilizzo di hard link per controllare se esiste gia'
 * un'istanza del programma e crea un file temporaneo
 * come funziona: prova a creare un file fittizio "dummy<pid>" univoco
 * per ogni instanza del programma e a creare un
 * hard link "dummy" (unico per tutte le instanze). Nel caso non e'
 * possibile creare l'hard link, significa che "dummy" punta gia' ad
 * un altro file di tipo "dummy<pid>" creato da un altra instanza del
 * programma. Se e' l'unica instanza attiva del programma genera un file
 * temporaneo e scrive qualcosa in esso (usare "lsof | grep <pid>" per
 * conoscere il nome del file, per vedere il suo contenuto invece
 * "less /proc/<pid>/fd/<file descriptor restituito da lsof>" e notare
 * come il contenuto sia aggiornato dopo fflush). In caso di
 * terminazione del processo con SIGINT (ctrl+c) o con SIGTERM, viene
 * eseguito un controllo per vedere se e' necessario cancellare il
 * file "dummy" creato, poiche' se non viene cancellato prima della
 * esecuzione di una nuova instanza, questa credera' che non sia
 * l'unica in esecuzione anche in caso contrario. Successivamente viene
 * eseguito il gestore di dafault per il segnale che indica di
 * terminare.
 * funzioni di rilievo da vedere nel man:
 * - link (link, unlink)
 * - file stream/descrittore file (tmpfile, open, close, fflush, fileno,
 *   fsync)
 * - processi (getpid)
 * - segnali (signal, raise, sigemptyset, sigaddset, sigprocmask)
 * - varie (TEMP_FAILURE_RETRY, errno).
 */

#include "only_one.h"

volatile sig_atomic_t t;

// se arriva il segnale di terminazione
// elimina il file "dummy", altrimenti un nuovo processo in esecuzione
// credera' di non essere la prima instanza
void sig_user(int signum) {

    // se ho creato il file dummy lo elimino
    if (t>-1) {
        printf("\nExit required by signal\nCleaning file\n");
        unlink("dummy");
    }

    // ripristino il segnale originale
    signal(signum,SIG_DFL);
    // e lo eseguo per far eseguire l'azione di default
    raise(signum);
}

int main(int argc, char *argv[]) {
    FILE *tmp_file;
    int the_file, p;
    char pid[15]="dummy";
    sigset_t block_sig;

    // ottieni il pid
    p=getpid();
    printf("Pid: %d\n",p);
    // e innestalo nella variabile pid che sara'  "dummy<pid>"
    // nota: 9 cifre dovrebbero essere sufficenti :-/
    sprintf(&pid[5],"%d",p);

    // crea il set di signali contenente SIGTERM e SIGINT
    sigemptyset(&block_sig);
    sigaddset(&block_sig,SIGTERM);
    sigaddset(&block_sig,SIGINT);

    // blocca SIGTERM e SIGINT
    sigprocmask(SIG_BLOCK,&block_sig,NULL);

    // cambia i gestori dei segnali di modo che prima di eseguire
    // l'azione di default venga fatta la pulizia necessaria
    signal(SIGTERM,sig_user);
    signal(SIGINT,sig_user);

    // crea il file se non esiste utilizzando le primitive di apertura
    // a file. Poiche' nel caso di interruzione da segnale segnalata con
    // errno=EINTR la chiamata a open non va a buon fine utilizza la
    // macro TEMP_FAILURE_RETRY definita nell'header che prova a
    // richiamare la funzione finche' la variabile errno non e' diversa
    // da EINTR
    the_file=TEMP_FAILURE_RETRY(open(pid,O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR));
    // se hai creato il file chiudilo (che non vuol dire eliminarlo)
    if (the_file==-1) {
        printf("Error !!! File %s, line %d\n",__FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }
    TEMP_FAILURE_RETRY(close(the_file));

    // prova a creare un hard link "dummy" al tuo file "dummy<pid>"
    t=link(pid,"dummy");
    // ora puntera' solo a "dummy "nel caso "dummy" non puntava a nulla
    // oppure sara' cancellato
    unlink(pid);

    // sblocca SIGTERM e SIGINT
    sigprocmask(SIG_UNBLOCK,&block_sig,NULL);

    // nel caso il link a "dummy" e' andato a buon fine e quindi sono
    // non ci sono altre instanze del programma attive
    if (t>-1) {
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

        // elimina il file "dummy" (la prima instanza "dummy<pid>" e'
        // gia' stata eliminata
        unlink("dummy");
    } else
        // c'era qualcuno
        printf("There is somebody...\n");

    exit(EXIT_SUCCESS);
}
