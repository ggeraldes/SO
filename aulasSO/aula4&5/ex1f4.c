
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void mostra(int s){

printf("\nola!!![sinal %d]\n", s);
alarm(3);
}

int main(int argc, char *argv[], char *envp[]) {   //numero de argumentos, argumentos

	int i;
	char str[40];

	//signal(SIGINT, SIG_DFL);
	//signal(SIGINT, SIG_INT);
	signal(SIGINT, mostra);
	signal(SIGALRM, mostra);
	printf("O meu PID é %d,\n", getpid());
	do{
	alarm(8);
	  printf("Nome? ");
	  fflush(stdout);
	  scanf("%s", str);

	  printf("Ola");
	  for(i=0; i<5; i++) {
	    printf("a"); fflush(stdout); sleep(1);
	  }
	  printf(" %s!!!\n", str);
	}while (strcmp(str,"sair")!=0);

	exit(0); //correu bem

}

