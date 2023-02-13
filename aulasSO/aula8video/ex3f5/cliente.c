#include "util.h"

int main(void) {  

    PEDIDO p;
    int fd, fdr, n, var=0;

    //criar named pipe (fifo)

    if(access(FIFO_CLI, F_OK)!=0){//verifica se o ficheiro "FIFO_SRV" existe, SE NAO EXISTE CRIA O FIFO

    mkfifo(FIFO_CLI, 0600);
    printf("Criei o fifo\n");
    } 

    
    do {
        //pedir dados...
        printf("\nnum1: ");
        scanf("%d", &p.num1);
        printf("\nnum2: ");
        scanf("%d", &p.num2);
        printf("\nop: ");
        scanf("%s", &p.op);

        //enviar pedido ...
        fd = open(FIFO_CLI, O_WRONLY);  //bloqueia - se não houver clientes
        n= write(fd, &p, sizeof(PEDIDO));
        close(fd);
        printf("Enviei...%d %c %d = %.2f (%d bytes)\n", p.num1, p.op, p.num2, p.res, n);

        //recebe resposta ....
        fdr = open(FIFO_SRV, O_RDONLY);
        n= read(fdr, &p, sizeof(PEDIDO));
        close(fdr);
        printf("Recebi...%d %c %d (%d bytes)\n", p.num1, p.op, p.num2,  n);


    }while(1); //s - sair


    //APAGAR FIFO
    unlink(FIFO_SRV);

        
    exit(0); //correu bem

}
