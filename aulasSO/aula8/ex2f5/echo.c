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
#define ECHO_FIFO "ECHOFIFO"
#define ECHO_PRODUTOR "ECHOPRODUTOR%d" // ECHOPRODUTOR1001

typedef struct {
    pid_t pid;
    char msg[100];
}  dataMSG;


void sair (int i,siginfo_t *info, void *s){
    printf ("Adeus");
    unlink(ECHO_FIFO);
    exit (1);
}

int main (){
    char nomeFifoProdutor[100];
    int size;
    dataMSG msg;
    struct sigaction sa;
        sa.sa_sigaction=sair;
    sigaction(SIGINT,&sa,NULL);


    if (mkfifo(ECHO_FIFO,0666) == -1)  {
       if (errno == EEXIST){
           printf ("Servidor em execução ou fifo já existe\n");
       }
       printf("Erro abrir fifo");
       return 1;
   }

   int fd = open (ECHO_FIFO,O_RDWR);
  // int fd = open (ECHO_FIFO,O_RDONLY);
    do{
        size = read (fd,&msg,sizeof(msg));
        if (size > 0){
                printf("\n recebi a msg [%s] com o tamanho [%d]",msg.msg,size);
                if (strcmp("sair\n",msg.msg)==0){
                    close(fd);
                    unlink (ECHO_FIFO);
                    return 1;
                }
                //resposta ao produtor
                sprintf (nomeFifoProdutor,ECHO_PRODUTOR,msg.pid);
                int fdEnvio = open (nomeFifoProdutor,O_WRONLY);
                msg.pid = getpid();
                int s2 = write (fdEnvio,&msg,sizeof(msg));
                close (fdEnvio);


        }
    } while (1);
   
}