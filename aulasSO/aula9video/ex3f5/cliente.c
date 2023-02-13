#include "util.h"

int main(void) {  

    PEDIDO p;
    int fd, fdr, n, var=0;
    char fifo[40];

    
    

    //criar named pipe (fifo)

    if(access(FIFO_SRV, F_OK)!=0){//verifica se o ficheiro "FIFO_SRV" existe, SE NAO EXISTE CRIA O FIFO

    fprintf(stderr, "[ERRO] O servidor nao esta a funcionar!\n");
    exit (1);
    
    } 
    if(access(FIFO_SRV1, F_OK)!=0){//verifica se o ficheiro "FIFO_SRV" existe, SE NAO EXISTE CRIA O FIFO

    fprintf(stderr, "[ERRO] O servidor nao esta a funcionar!\n");
    exit (1);
    
    } 

    p.pid=getpid();
    sprintf(fifo, FIFO_CLI, p.pid); //acrescentar ao nome do fifo o pid
    mkfifo(fifo, 0600);
    printf("Criei o fifo\n");

    fd = open(FIFO_SRV, O_WRONLY);
    printf("Abri o fifo do servidor - '%s'\n", FIFO_SRV);
    do {
        //pedir dados...
        printf("\nnum1: ");
        scanf("%d", &p.num1);
        printf("\nnum2: ");
        scanf("%d", &p.num2);
        printf("\nop: ");
        scanf("%s", &p.op);

        //enviar pedido ao SRV ...
        n= write(fd, &p, sizeof(PEDIDO));
        printf("Enviei...%d %c %d (%d bytes)\n", p.num1, p.op, p.num2, n);
        if(p.op=='c' || p.op=='s')
            var=1;

        //recebe resposta do SRV....
        if(var==0){
        fdr = open(fifo, O_RDONLY);
        n= read(fdr, &p, sizeof(PEDIDO));
        close(fdr);
        printf("Recebi...%d %c %d = %.2f (%d bytes)\n", p.num1, p.op, p.num2, p.res,  n);
        }


    }while(p.op!='c' && p.op!='s'); //c - sair
    close(fd);

    //APAGAR FIFO
    unlink(fifo);

        
    exit(0); //correu bem

}
