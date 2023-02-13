//ex5
 

 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#define SERVIDOR_FIFO "SERVIDOR_FIFO"
#define CLIENT_FIFO "ECHOPRODUTOR%d" // ECHOPRODUTOR1001

char CLIENT_FIFO_FINAL[100];

typedef struct {
    pid_t pid;
    char operador;
    int num1;
    int num2;
}  dataMSG;

typedef struct{
    float res;
}dataRpl;

void handler_sigalarm(int s,siginfo_t *t, void *v){
                    unlink (SERVIDOR_FIFO);
                    printf("\nadeus\n");
                    exit (1);

}
void funcExit(int s, siginfo_t *i, void *v){
    //handler_sigalarm(1,);
}
void constroiNomeFifo(char *nome, int pid){
    sprintf(CLIENT_FIFO_FINAL, CLIENT_FIFO, pid);
}
int main (){

    struct sigaction sa;
    sa.sa_sigaction=handler_sigalarm;
    sigaction(SIGINT, &sa, NULL);

    dataMSG mensagemRecebida;
    dataRpl resposta;
   
    

    int nfd;
    fd_set read_fds;
    struct timeval tv;
    char xpto[30];

   
   if (mkfifo(SERVIDOR_FIFO,0666) == -1)  {
       if (errno == EEXIST){
           printf ("Servidor em execução ou fifo já existe\n");
       }
       printf("Erro abrir fifo");
       return 1;
   }

int fdRecebe = open (SERVIDOR_FIFO,O_RDWR);
  // int fd = open (ECHO_FIFO,O_RDONLY);

    if(fdRecebe==-1){ printf("ERRO");return 1;}
    do{

        tv.tv_sec = 5;
        tv.tv_usec = 0;
        FD_ZERO (&read_fds);
        FD_SET (0, &read_fds);
        FD_SET (fdRecebe, &read_fds);
        int maior= fdRecebe+1;

        nfd = select(maior,&read_fds, NULL, NULL, &tv);
        //printf("time -> %d\n",nfd );
        if(__FD_ISSET(0,&read_fds)){
        fgets(xpto, sizeof(xpto),stdin);
        if(strcmp(xpto,"terminar\n")==0)
        exit(1);
        else
        printf("COmando invalido");
    }

    if(FD_ISSET(fdRecebe, &read_fds)){
        int size=read(fdRecebe, &mensagemRecebida, sizeof(mensagemRecebida));
        if(size>0){
        if(mensagemRecebida.operador=='+')
            resposta.res=mensagemRecebida.num1 + mensagemRecebida.num2;
                else if(mensagemRecebida.operador=='.')
                kill(getpid(), SIGINT);
            //resposta
            sprintf (CLIENT_FIFO_FINAL,CLIENT_FIFO,mensagemRecebida.pid);
               
                int fdEnvio = open (CLIENT_FIFO_FINAL,O_WRONLY);
                int s2 = write (fdEnvio,&resposta,sizeof(resposta));
                close (fdEnvio);

        
        }

    }


   
    } while (1);
   
}