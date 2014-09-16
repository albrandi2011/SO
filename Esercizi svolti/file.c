//lettura e scrittura file
// r 	file di testo in sola lettura.
// w 	file di testo in sola scrittura. Se esiste ne è cancellato il contenuto altrimenti viene creato
// r+ 	file di testo in lettura e scrittura.
// w+ 	Se il file non esiste, lo crea e lo apre in lettura e scrittura. Se esiste, lo cancella e lo apre in lettura     
//        e  scrittura.
// a 	Apre il file in sola scrittura, facendo in modo che si possa solo aggiungervi del contenuto, alla fine. Se il 
//      file non esiste, viene creato.
// a+ 	Come a, solo che è abilitata anche la lettura.
// b 	se una b viene aggiunta alle modalità precedenti si indica che il file è binario. Ossia:

// rb, wb, ab, e r+b, w+b, a+b equivalenti a rb+, wb+, ab+



#include<stdlib.h>
#include<stdio.h>


int main()
{
 FILE *fp;
 int x,l;
  char buf[200];
  char *res;

 x=rand()%100+1;

 fp=fopen("ciao.txt","a");
 
 if(fp==NULL)
  {
   printf("Errore file\n");
   exit(1);
  }

 fprintf(fp,"%d\n",x);
 printf("ho scritto nel file %d\n",x);
 fclose(fp);

 fp=fopen("ciao.txt","r");

 while(1) {
    res=fgets(buf, 200, fp);
    if( res==NULL )
      break;
    printf("%s", buf);
  }

 fclose(fp);
}
