//echo
 

 

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

int main (){

    struct sigaction sa;
    sa.sa_sigaction=handler_sigalarm;
    sigaction(SIGINT, &sa, NULL);

    dataMSG mensagemRecebida;
    dataRpl resposta;
   
    char CLIENT_FIFO_FINAL[100];

    if (mkfifo(SERVIDOR_FIFO,0666) == -1)  {
       if (errno == EEXIST){
           printf ("Servidor em execução ou fifo já existe\n");
       }
       printf("Erro abrir fifo");
       return 1;
   }

   int fdRecete = open (SERVIDOR_FIFO,O_RDWR);
  // int fd = open (ECHO_FIFO,O_RDONLY);
    do{
        int size = read (fdRecete,&mensagemRecebida,sizeof(mensagemRecebida));
        if (size > 0){
            if(mensagemRecebida.operador == '+')
                resposta.res=mensagemRecebida.num1 + mensagemRecebida.num2;
                else if(mensagemRecebida.operador=='.')
                kill(getpid(), SIGINT);

                //resposta ao clientess
                sprintf (CLIENT_FIFO_FINAL,CLIENT_FIFO,mensagemRecebida.pid);
               
                int fdEnvio = open (CLIENT_FIFO_FINAL,O_WRONLY);
                int s2 = write (fdEnvio,&resposta,sizeof(resposta));
                close (fdEnvio);

                


        }
    } while (1);
   
}