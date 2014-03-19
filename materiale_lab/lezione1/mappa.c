// mappa.cpp:
//Costruire la mappa di bit 
// si supponga di blocchi di 1 K

#include <stdio.h>
#include <malloc.h>
void Inserisci(char *mappa,unsigned long mem);
void Elimina(char *mappa);
void visualizza(char *mappa,unsigned long mem);
char scelta[2];
int main(int argc, char* argv[])
{char *mappa;
 unsigned long i,mem;
 printf("dai la quantit? di memoria disponibile in k:");
 scanf("%d",&mem);
 mappa=(char *)malloc(sizeof(char)*mem);
 for(i=0;i<mem;i++) mappa[i]=0;
 
 do{
	 do{printf("Per inserire  un processo : I\nPer eliminare il processo : E\nPer Uscire                            : s\nScelta:");
        scanf("%s",scelta);
		if( (scelta[0]!='I') && (scelta[0]!='E') && (scelta[0]!='s')) 
			printf("Scelta non consentita\n\n");
		}while( (scelta[0]!='I') && (scelta[0]!='E') && (scelta[0]!='s'));    	
     if(scelta[0]=='I') Inserisci(mappa,mem);
     if(scelta[0]=='E') Elimina(mappa);
 }while(scelta[0]!='s');
 return 0;

}

void Inserisci(char *mappa,unsigned long mem)
{unsigned long i,error;
 unsigned long BIni;
 unsigned long Lungh;
 printf("Dai il blocco iniziale della memoria del Processo:");scanf("%d",&BIni);
 printf("Dai la memoria occupata dal Processo:");scanf("%d",&Lungh);
 if( (BIni>mem) || (BIni+Lungh>mem)) 
	printf("Blocco di memoria non consentito!!!! processo non eseguibile\n");
 else
 { error=0;
   for(i=BIni;i<BIni+Lungh && error ==0;i++)
	if(mappa[i]==1) error=1;
   if(error==0)
    for(i=BIni;i<BIni+Lungh;i++) mappa[i]=1;
   else
      printf("Blocco di memoria occupato\n");    
   visualizza(mappa,mem);
 }
}

void Elimina(char *mappa)
{
}

void visualizza(char *mappa,unsigned long mem)
{unsigned long i,j;
 for(i=0;i<mem;i=i+8)
 {printf("mapp[%d-%d]",i,i+7);
  for(j=0;j<=7;j++)
	  if((i+j)<mem) printf("%d",mappa[i+j]);
   printf("\n");
 }
 
}