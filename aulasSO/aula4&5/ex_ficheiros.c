
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[], char *envp[]) {   //numero de argumentos, argumentos

	int i, fd;
	 printf("pid=[%d].\n", getpid());
	close(1);
	fd = open("dados.bin", O_CREAT | O_TRUNC | O_WRONLY, 0644 );
	//execl(...); //todo o seu output e redirecionado para o ficheiro

if(fd>0){ //erro=-1
        printf("INICIO...\n");
	for(i=0; i<20; i++) {		 
	printf("." );
	fflush(stdout);	
 	sleep(1);       
	 }
	printf("\nFIM...\n");
 	 write(fd,&i, sizeof(int));
	write(fd,"joao", 4);
	write(fd, "maria", 5);

	close(fd);
}
	exit(3); //correu bem

}

