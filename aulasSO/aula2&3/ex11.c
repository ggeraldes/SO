

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[], char *envp[]) {   //numero de argumentos, argumentos

	int i;
	char *str;
	if(argc!=3){
	  printf("[ERRO] Nr. de args!\n");
	  exit(3); //correu mal - 3 (numero de argumentos invalido
	}

        printf("[DEBUG] INICIO...\n");
	for(i=0; i<argc; i++) {		 
	printf("ARG[%d]='%s'",i, argv[i] );
        }
	i = atoi(argv[2]); //array to int
	printf("%d", ++i);
/*	for(i=0;envp[i]!=NULL; i++) { //ENQUANTO ENVP NAO TIVER NULOS
        printf("ENVP[%d]='%s'",i, envp[i] );
        }*/
	str =getenv("NOME");
	printf("O meu nome e %s...", str);
	 printf("[DEBUG] FIM!! \n");
        return (0);
	
	exit(0) //correu bem

}
