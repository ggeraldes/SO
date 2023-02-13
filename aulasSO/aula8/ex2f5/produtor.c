//produtor

 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#define ECHO_FIFO "ECHOFIFO"
#define ECHO_PRODUTOR "ECHOPRODUTOR%d" // ECHOPRODUTOR1001

typedef struct {
    pid_t pid;
    char msg[100];
}  dataMSG;


int main (){
    char nomeFifoProdutor[100];
    sprintf (nomeFifoProdutor,ECHO_PRODUTOR,getpid());
     if (mkfifo(nomeFifoProdutor,0666) == -1)  {
       printf("Erro abrir fifo");
       return 1;
   }

    dataMSG msg,resposta;

    do {
        printf("\nO que pretende enviar?");
        fgets(msg.msg,sizeof(msg.msg),stdin);
        if (strcmp(msg.msg,"terminar\n")==0){
            unlink(nomeFifoProdutor);
            return 1;
        }
        int fd_echo_fifo = open (ECHO_FIFO,O_WRONLY);
        if (fd_echo_fifo == -1){
            printf("Erro o servidor não está a correr");
            return 1;
        }    
        msg.pid = getpid();
        int s = write (fd_echo_fifo,&msg,sizeof(msg));
        if (s<=0)
            printf("erro no envio\n");  
        close (fd_echo_fifo);

        //receber a msg de volta
        int fd_resposta = open (nomeFifoProdutor,O_RDONLY);
            int s2 = read (fd_resposta,&resposta,sizeof(resposta));
            printf ("Resposta [%s] de [%d] pid",resposta.msg,resposta.pid);
        close (fd_resposta);

    }while (1);
}