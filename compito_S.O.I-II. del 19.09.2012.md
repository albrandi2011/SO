# ESAMI DI LABORATORIO SISTEMI OPERATIVI DEL 18.09.2012

## Esercizio 1(15/30) (Analisi E Scelte Fatte 6, Gestione Thread E Sincronizzazione 6, Codice 3)

* Un gruppo di N amici con N>4, decidono di far un torneo di ping pong. Per fare ciò affittano M con M>2 tavoli da ping pong con relative racchette.
* Quindi scelti i primi M*2 giocatori essi giocano e finita la partita si mettono a turno ad un altro tavolo scelto a caso .
* La fine del torneo è decretata da un evento esterno che comunque fa terminare le partite in corso.
* Il vincitore sarà colui che ha vinto più partite.
* Si considerino le persone come thread utilizzando le variabili condivise solo per aggiornare i punteggi, mentre per la sincronizzazione utilizzare esclusivamente mutex o semafori.
* La vita dei giocatori è questa.
* Finchè non è scaduto il tempo
* Scelgo il campo random e cerco di acquisire uno dei due posti.
* Se sono il primo giocatore del campo esimo:

1. Aspetto che il secondo giocatore si presenti e gioco.

* Se sono il secondo giocatore del campo esimo

1. si gioca

2. aggiorno il punteggio, il mio se sono il vincitore viceversa il punteggio del mio avversario.

3. Rilascio il tavolo e faccio rilasciare il tavolo anche al mio avversario

Si analizzi la simulazione commentando le scelte fatte e si sviluppi un programma in C che simuli la situazione descritta.

## Esercizio 2(15/30) Per gli studenti di Sistemi Operativi e Sistemi Operativi II

Scrivere un programma multi processo in linguaggio C in ambiente linux in cui si simuli l’esercizio precedente considerando questa volta i tavoli come processi distinti.

Questa volta si conosce il numero di partite da effettuare in ogni tavolo.

In pratica il processo principale dati un input N e Me K con N numero di giocatori M i tavoli e K numero di partite da effettuare per ogni tavolo, si generino M processi, uno per ogni tavolo.

Il processo esimo simulerà le K partite dei vari giocatori aggiornando il punteggio dei vari giocatori.
Si suggerisce di utilizzare un vettore di N booleani per tenere presente chi sta giocando e chi no.
