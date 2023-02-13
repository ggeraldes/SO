
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[], char *envp[]) {   //numero de argumentos, argumentos

	int a ,i, tempo;
	char *str;
	if(argc!=3){
	  printf("[ERRO] Nr. de args!\n");
	  exit(3); //correu mal - 3 (numero de argumentos invalido
	}

        printf("[DEBUG] INICIO...\n");
        a=atoi(argv[1]);
        str=getenv("TEMPO");
	if(str==NULL){
	printf("'TEMPO' não definido, tempo=1s\n");
	tempo=1;
	}else
	tempo = atoi(str);

	fflush(stdout);
	for(i=0; i<a; i++) {		 
	printf("%s", argv[2] );	fflush(stdout);	
 	sleep(tempo);       
	 }
	printf("\n[DEBUG]FIM...\n");
    
	
	exit(0); //correu bem

}

