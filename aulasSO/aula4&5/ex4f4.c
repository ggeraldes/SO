
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

void nova(int s, siginfo_t *info, void *context){

	printf ("\nTIMEOUT!!! (Sinal = %d, PID = %d, Valor = %d)\n",
	   s, info->si_pid, info->si_value);
	if(info ->si_pid==0)
	  erradas++;

}

int main() {

	int i, num1, num2;

	srand((unsigned int)time(NULL));

#if 0
	//signal(SIGALRM, mostra);
	struct sigaction act;
	act.sa_handler = mostra;
	act.sa_flags=0; //SA_RESTART
	sigaction(SIGALRM, &act,NULL);
#else
	struct sigaction act;
	act.sa_sigaction = nova;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGALRM, &act,NULL);
#endif
	
	printf("Indique os resultados das somas [%d]\n", getpid());
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

