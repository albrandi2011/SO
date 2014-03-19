// mappa.cpp:
//Costruire la mappa di bit 
// si supponga di blocchi di 1 K

#include <stdio.h>
#include <malloc.h>
void Inserisci(unsigned char *mappa,unsigned long mem);
void Elimina(unsigned char *mappa);
void visualizza(unsigned char *mappa,unsigned long mem);
char scelta[2];
int main(int argc, char* argv[])
{unsigned char *mappa;
 unsigned long i,mem,memT;
 printf("dai la quantit? di memoria disponibile in k:");
 scanf("%d",&mem);
 memT=(mem/8)+1;
 mappa=(unsigned char *)malloc(sizeof(char)*memT);
 for(i=0;i<memT;i++) mappa[i]=0;
 
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

void Inserisci(unsigned char *mappa,unsigned long mem)
{unsigned long i,j,error;
 unsigned long BIni,BiniT,offset;
 unsigned long Lungh;
 unsigned char a=128;
 printf("Dai il blocco iniziale della memoria del Processo:");scanf("%d",&BIni);
 printf("Dai la memoria occupata dal Processo:");scanf("%d",&Lungh);
 if( (BIni>mem) || (BIni+Lungh>mem)) 
	printf("Blocco di memoria non consentito!!!! processo non eseguibile\n");
 else
 { error=0;
   BiniT=BIni/8;
   offset=BIni-(BiniT*8);
   a=a>>offset;
   j=BiniT;
   for(i=BIni;i<BIni+Lungh && error ==0;i++)
   {if( (i>BIni)&&(i%8)==0) {j++; a=128;}
	if( (mappa[j]&a)>0) error=1;
    a=a>>1;
	
   }
   if(error==0)
   {a=128;
    a=a>>offset;
	j=BiniT;
    for(i=BIni;i<BIni+Lungh;i++) 
	{if( (i>BIni)&&(i%8)==0) {j++; a=128;}
	 mappa[j]=mappa[j]|a;
	 a=a>>1;
	 
	 
	}
   }
   else
      printf("Blocco di memoria occupato\n");    
   visualizza(mappa,mem);
 }
}

void Elimina(unsigned char *mappa)
{
}

void visualizza(unsigned char *mappa,unsigned long mem)
{unsigned long i,j;
 unsigned long memT=(mem/8)+1;
unsigned char a=128;
 for(i=0;i<memT;i++)
 {printf("mapp[%d-%d]",i*8,(i*8)+7);
  a=128;
  for(j=0;j<=7;j++)
  { if( (mappa[i]&a)>0) printf("1");
	else printf("0");
     a=a>>1;	
  }
  printf("\n");
 }
 
}