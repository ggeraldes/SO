
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


int main(int argc, char *argv[], char *envp[]) {   //numero de argumentos, argumentos

	int i;
	 printf("pid=[%d].\n", getpid());
        printf("INICIO...\n");
	for(i=0; i<20; i++) {		 
	printf("." );
	fflush(stdout);	
 	sleep(1);       
	 }
	printf("\nFIM...\n");
    
	
	exit(3); //correu bem

}

