
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {   //numero de argumentos, argumentos

	int i;
        printf("[DEBUG] INICIO...\n");
	for(i=0; i<argc; i++) {		 
	printf("ARG[%d]='%s'",i, argv[i] );
        }
	i = atoi(argv[2]); //array to int
	printf("%d", ++i);
	printf("[DEBUG] FIM!! \n");
        return (0);


}
