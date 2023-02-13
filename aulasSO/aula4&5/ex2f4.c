
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

int certas=0, erradas=0;

void mostra(int s){

printf("\nDemoraste muito!\n");
erradas++;
if(erradas>=3){
 printf("\nResultado: certas=%d, erradas=%d\n", certas, erradas);
 exit(3);
}
alarm(10);
}

int main() {

	int i, num1, num2;
	char str[40];

	srand((unsigned int)time(NULL));

	//signal(SIGALRM, mostra);
	struct sigaction act;
	act.sa_handler = mostra;
	act.sa_flags=0;
	sigaction(SIGALRM, &act,NULL);
	printf("Indique os resultados das somas\n");
	do{ i=-1;
	  num1=rand() % 101;
	  num2=rand() % 101;
	  printf("\n%d+%d? ", num1,num2);
	  alarm(10);
	  fflush(stdout);
	  if(scanf("%d",&i)==1){
	    if( i==(num1+num2)){
	     printf("\nACERTOU!\n");
	     certas++;
	     alarm(0);
	    }
	    else{
             printf("\nERROU!\n");
             erradas++;
            }

	  }

	}while (erradas<3);

	printf("\nResultado: certas=%d, erradas=%d\n", certas, erradas);

	exit(0); //correu bem

}

