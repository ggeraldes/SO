#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[], char *envp[]) {   //numero de argumentos, argumentos


	char str[40];
	int continua=1,pid, res, estado;
	int canal[2], n;

	pid=getpid();

	while(continua){
	  printf("[%5d]COMANDO: ", pid);
	  fflush(stdout);
	  scanf("%s",str);
	  if(strcmp(str,"sair")==0){
	    continua=0;
	  }else{
	    pipe(canal);
	    res=fork();
	    if(res == 0){  // FILHO
	      pid = getpid();

	      close(canal[0]);// extremidade de leitura
	      close(1); 
	      dup(canal[1]);//coloca a extremidade de escrita no descritor 1
		  close (canal[1]);

  	      execl(str, str, NULL);
              printf("[%5d]Não consegui executar a aplicação\n", pid);
	      exit(7);
	    }
	  printf("[%d] PAI:Criei o filho PID=%d..\n", pid,res);
	close(canal[1]);//extremidade de escrita
	while((n=read(canal[0], str, 39))>0){
	str[n]='\0';
		 printf("[%5d] PAI: Recebi '%s'\n",pid, str);

	}
	close(canal[0]); //extremidade de leitura
	  //esperar pelo filho
	  wait(&estado);
	  if(WIFEXITED(estado)){ //se saíu
	    printf("[%5d] PAI: O filho saiu com %d\n", pid, WEXITSTATUS(estado)); //tipo de erro
	  }
	  }
	}
	exit(0); //correu bem

}

