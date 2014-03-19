/*
 * per compilare: gcc thread.c -Wall -lpthread -o thread
 * per eseguire: ./thread
 * cosa fa: genera piu' thread all'interno dello stesso processo, che
 * eseguono funzioni differenti, mostra un esempio di barriera per
 * sincronizzare i processi, di chiave con valore univoco per ogni
 * thread e della funzione pthread_once per eseguire una funzione
 * solo una volta nel codice condiviso tra thread.
 * come funziona: il thread iniziale del processo alloca le risorse
 * per la chiave, le barriere per la sincronizzazione e per passare
 * i parametri di ingresso, poi genera nuovi thread che seguono una
 * funzione o un'altra a seconda se il loro turno nella generazione
 * era pari o dispari. Il thread originario infine aspetta che tutti
 * gli altri finiscano, in maniera simile a quanto visto per i processi
 * con fork e wait, per evitare spreco di risorse. I thread oltre a
 * concorrere per l'esecuzione di una funzione, che verra' eseguita
 * solo un volta, generano ciascuno un proprio valore associato ad una
 * stessa chiave condivisa e si sincronizzano tramite barriere.
 * funzioni di rilievo da vedere nel man:
 * - thread (pthread_create, pthread_join, phtread_exit, pthread_once,
 *   pthread_equal, pthread_detach)
 * - barriere (pthread_barrier_init, pthread_barrier_wait,
 *   pthread_barrier_destroy)
 * - chiavi (pthread_setspecific, pthread_getspecific,
 *   pthread_key_delete, pthread_key_create).
 */

#include "thread.h"

pthread_t daddy;
int res[MAX_THREADS];
pthread_key_t pkey;
pthread_barrier_t pbarrier1, pbarrier2;
pthread_t who_call_me_first;
pthread_once_t once_control=PTHREAD_ONCE_INIT;

// funzione eseguita una sola volta
void execute_once(void) {
    // registra chi e' l'unico thread che ha chiamato la funzione
    who_call_me_first=pthread_self();
}

// da chiamare all'uscita in caso il valore associato alla chiave non
// sia nullo
void free_key(double *val) {
    printf("Cleaning key %f\n",*val);
    free(val);
}

// funzione da eseguire per i thread di indice pari
int *do_struct_arg(arg_struct *in) {

    double *key_val;
    double *out_key_val;

    // prova ad eseguire la funzione se non e' stata gia' eseguita
    pthread_once(&once_control,execute_once);

    // mostra gli argomenti con cui e' stato chiamato il thread
    printf("Thread %d present and called with string %s!\n",in->arg_int,in->arg_string);
    // memorizza il valore di ritorno al posto predefinito
    res[in->arg_int]=4;

    // alloca uno spazio di memoria da associare alla propria chiave
    key_val=(double *)malloc(sizeof(double));
    *key_val=40/(double)(rand()%40+1);
    // e associalo alla chiave
    pthread_setspecific(pkey,(void *)key_val);
    printf("Thread %d - setting own key to %f\n",in->arg_int,*key_val);

    // attendi che gli altri abbiano eseguito le stesse istruzioni
    // (il thread originale fa qualcosa di differente)
    pthread_barrier_wait(&pbarrier1);
    pthread_barrier_wait(&pbarrier2);

    // controlla se il thread corrente e' quello che ha chiamato
    // la funzione eseguibile solo una volta
    if (pthread_equal(pthread_self(),who_call_me_first))
        printf("Thread %d executed 'executed_once' first!\n",in->arg_int);
    else
        printf("Thread %d did'nt execute 'executed_once' first!\n",in->arg_int);

    // ottieni il valore proprio del thread associato alla chiave
    out_key_val=(double *)pthread_getspecific(pkey);
    printf("Thread %d - retrieving key %f\n",in->arg_int,*out_key_val);

    // ritorna il puntatore al valore di risultato
    return (&res[in->arg_int]);
}

// funzione da eseguire per i thread dispari e per il thread originario
int *do_int_arg(int *in) {

    // prova ad eseguire la funzione se non e' stata gia' eseguita
    pthread_once(&once_control,execute_once);

    // se sono il thread originale mostralo su terminale
    if (pthread_equal(daddy,pthread_self())) {
        printf("I'm daddy!\n");
        // e dimmi se ho eseguito la funzione che puo' essere lanciata
        // solo una volta
        if (pthread_equal(pthread_self(),who_call_me_first))
            printf("Daddy executed 'executed_once' first!\n");
        else
            printf("Daddy did'nt execute 'executed_once' first!\n");
    }
    // altrimento
    else {
        // attendi che tutti abbiano eseguito le istruzioni necessarie
        printf("Thread %d present!\n",*in);
        pthread_barrier_wait(&pbarrier1);
        pthread_barrier_wait(&pbarrier2);
        // e dimmi se ho eseguito la funzione che puo' essere lanciata
        // solo una volta
        if (pthread_equal(pthread_self(),who_call_me_first))
            printf("Thread %d executed 'executed_once' first!\n",*in);
        else
            printf("Thread %d did'nt execute 'executed_once' first!\n",*in);
    }

    // se sono il thread originario ritorna un puntatore a NULL
    if (pthread_equal(daddy,pthread_self()))
        return NULL;
    // altrimenti il puntatore al valore corretto
    else {
        res[*in]=10;
        pthread_exit((void *)&res[*in]);
    }
}

int main(int argc, char *argv[]) {

    pthread_t thread[MAX_THREADS];
    arg_struct *args_with_struct;
    int *args_only_int;
    int ks, ki;
    int i, t, pos;
    char dummy[]="defewfkls";
    int *res;

    // ricordati chi e' il thread originario
    daddy=pthread_self();
    // crea la chiave
    pthread_key_create(&pkey,(void (*)(void *))free_key);
    // e le barriere
    pthread_barrier_init(&pbarrier1,NULL,MAX_THREADS+1);
    pthread_barrier_init(&pbarrier2,NULL,MAX_THREADS+1);

    // alloca lo spazio per gli argomenti dei thread pari
    ks=MAX_THREADS/2;
    args_with_struct=(arg_struct *)malloc(sizeof(arg_struct)*ks);

    // e dispari
    ki=MAX_THREADS-ks;
    args_only_int=(int *)malloc(sizeof(int)*ks);

    // genera i thread
    for (i=0;i<MAX_THREADS;i++) {
        t=i%2;
        pos=i/2+i%2;
        // se sono pari
        if (!t) {
            // riempi la struttura da passare in input
            args_with_struct[pos].arg_int=i;
            args_with_struct[pos].arg_string=dummy;
            // e genera il thread relativo
            if (pthread_create(&thread[i],NULL,(void *(*)(void *))do_struct_arg,(void *)&args_with_struct[pos]))
                printf("Error %d\n",i);
        }
        // se sono dispari
        else {
            // riempi la struttura da passare in input (piu' semplice)
            args_only_int[pos]=i;
            // e genera il thread relativo
            if (pthread_create(&thread[i],NULL,(void *(*)(void *))do_int_arg,(void *)&args_only_int[pos]))
                printf("Error %d\n",i);
        }
    }
    // il thread originario esegue pure la funzione eseguita dai thread dispari
    do_int_arg(NULL);

    // attendi il completamento alla barriera per sincronizzare
    // e avvisa
    pthread_barrier_wait(&pbarrier1);
    printf("*** Other instruction block ***\n");
    pthread_barrier_wait(&pbarrier2);

    // attendi che tutti gli altri thread abbiano finito
    // simile al wait del fork, per evitare di produrre "thread zombie"
    // e spreco di risorse, a meno che non venga utilizzato
    // pthread_detach e il thread abbia gli attributi corretti
    for (i=0;i<MAX_THREADS;i++) {
        if (pthread_join(thread[i],(void **)&res))
            printf("Error %d\n",i);
        // e dammi il valore di uscita
        printf("Thread %d finished with value %d\n",i,*res);
    }

    // rilascia le risorse associate alla barriera
    printf("Cleaning barrier\n");
    pthread_barrier_destroy(&pbarrier1);
    pthread_barrier_destroy(&pbarrier2);

    // per la chiave
    printf("Cleaning the key\n");
    pthread_key_delete(pkey);

    // e la memoria per il passaggio dei parametri di input
    free(args_with_struct);
    free(args_only_int);

    printf("All done\n");
    exit(EXIT_SUCCESS);
}
