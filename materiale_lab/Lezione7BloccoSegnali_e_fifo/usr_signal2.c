/*
 * per compilare: gcc usr_signal2.c -Wall -O3 -o usr_signal2
 * per eseguire: ./usr_signal2
 * cosa fa: all'invio dei segnali relativi mostra un messaggio sullo
 * schermo, durante l'esecuzione del signal handler determinati
 * segnali vengono bloccati
 * come funziona: dopo l'esecuzione del comando viene inviato il segnale
 * SIGUSR1 al programma (kill -s SIGUSR1 <pid>). Se il segnale SIGUSR2
 * viene inviato prima di digitare invio all'interno del signal handler
 * per SIGUSR1, questo viene sospeso e bloccato. Dopo il primo invio
 * verra' sbloccato. La ricezione di SIGUSR2 dopo il primo invio viene
 * invece gestita immediatamente. Attenzione, la funzione printf non e'
 * sicura con i segnali.
 * funzioni di rilievo da vedere nel man:
 * - segnali (signal, sigemptyset, sigaddset, sigprocmask).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sig_user_print1(int signum);
void sig_user_print2(int signum);
int main(int argc, char *argv[]);

void sig_user_print1(int signum)
{sigset_t block_sig;
 sigemptyset(&block_sig);// inizializza l'insieme vuoto dei segnali bloccati
 sigaddset(&block_sig,SIGUSR2);// aggiungi SIGUSR2
 sigprocmask(SIG_BLOCK,&block_sig,NULL);// blocca SIGUSR2
 printf("SIGUSR2 blocked in SIGUSR1\n");// avvisa della ricezione di SIGUSR1
 printf("SIGUSR1 sent...\n");
 printf("Press enter\n");
 getchar();// attendi invio
 printf("SIGUSR2 unlocked...\n");
 sigprocmask(SIG_UNBLOCK,&block_sig,NULL);// sblocca SIGUSR2
 getchar();// attendi invio
 printf("Exit SIGUSR1\n");
}

void sig_user_print2(int signum)
{printf("SIGUSR2 sent...\n");} // avvisa dell'invio del segnale

int main(int argc, char *argv[])
{// puntatori alle funzioni dei signal handler
 void (*sig_old1)(int);
 void (*sig_old2)(int);
 printf("My pid is: %d\n",getpid());// ottieni il pid da utilizzare col comando kill
 // sostituisci i signal handler
 sig_old1=signal(SIGUSR1,sig_user_print1);
 sig_old2=signal(SIGUSR2,sig_user_print2);
 printf("Wait or type enter to exit...\n");
 getchar();// attendi un carattere (invio)
 // ripristina i signal handler originari
 signal(SIGUSR1,sig_old1);
 signal(SIGUSR2,sig_old2);
 exit(EXIT_SUCCESS);
}