#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#define SERVER_FIFO "SERVIDOR1"
#define CLIENT_FIFO "CLIENTE%d"
char CLIENT_FIFO_FINAL[100];
typedef struct {
    pid_t pid;
    char operador;
    int num1;
    int num2;
}  dataMSG;
typedef struct servidor
{
    float res;
}dataRPL;

int main (){
     dataMSG mensagem;
     dataRPL resposta;
     mensagem.pid = getpid();
    int fd_envio,fd_resposta;
    sprintf(CLIENT_FIFO_FINAL,CLIENT_FIFO,getpid());
    if (mkfifo(CLIENT_FIFO_FINAL,0666) == -1)  {
       printf("Erro abrir fifo");
       return 1;
   }
    do{
    printf("introduza o operador\n");
    scanf(" %c",&mensagem.operador);
    if (mensagem.operador=='.'){// SE FOR .
         fd_envio = open(SERVER_FIFO,O_WRONLY);
         int size = write(fd_envio,&mensagem,sizeof(mensagem));
         close(fd_envio); unlink(CLIENT_FIFO_FINAL); exit(1);    
    } else {
        printf("introduza o numero 1\n");
        scanf("%d",&mensagem.num1);
        printf("introduza o numero 2\n");
        scanf("%d",&mensagem.num2);  
    }
     // printf("antes open\n");
      fd_envio = open(SERVER_FIFO,O_WRONLY);
      //  printf("depois open\n");
      int size = write(fd_envio,&mensagem,sizeof(mensagem));
      close (fd_envio);
      //RECEBER O CALCULO
      fd_resposta = open(CLIENT_FIFO_FINAL,O_RDONLY);
      //fd_resposta = open(CLIENT_FIFO_FINAL,O_RDWR);
      if (fd_resposta == -1 ) printf ("erro abrir o fifo do cliente");
     int siz2 = read(fd_resposta,&resposta,sizeof(resposta));  
     close (fd_resposta);  
     printf("\nResultado do calculo - [%f]\n",resposta.res);
    }while(1); }