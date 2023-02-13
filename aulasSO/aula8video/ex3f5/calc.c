#include "util.h"

int main(void) {  

    PEDIDO p;
    int fd, fdr, n, var=0;

    //criar named pipe (fifo)

    if(access(FIFO_SRV, F_OK)!=0){//verifica se o ficheiro "FIFO_SRV" existe, SE NAO EXISTE CRIA O FIFO

    mkfifo(FIFO_SRV, 0600);
    printf("Criei o fifo\n");
    } 

    fd = open(FIFO_SRV, O_RDONLY);  //bloqueia - se não houver clientes
    do {

        //ler pedido ...
        n= read(fd, &p, sizeof(PEDIDO));
        printf("Recebi...%d %c %d (%d bytes)\n", p.num1, p.op, p.num2,  n);

        //fazer a conta ....
        if(p.op=='+'){
            p.res=p.num1+p.num2;var=1;}
        else if(p.op=='-'){
            p.res=p.num1-p.num2;var=1;}
        else if(p.op=='*'){
            p.res=p.num1*p.num2;var=1;}
        else if(p.op=='/'){
            p.res=p.num1/p.num2;var=1;}
        else if(p.op=='s')
            printf("\nAdeus!\n");
        else
            printf("\nOperador Incorreto!!\n");

        //enviar resposta ....
        if(var==1){
            fdr = open(FIFO_CLI, O_WRONLY);  //bloqueia - se não houver clientes
            n= write(fdr, &p, sizeof(PEDIDO));
            close(fdr);
            printf("Enviei...%d %c %d = %.2f (%d bytes)\n", p.num1, p.op, p.num2, p.res, n);
        }
        var=0;

    }while(p.op!='s'); //s - sair
    close(fd); //fechar FIFO


    //APAGAR FIFO
    unlink(FIFO_SRV);

        
    exit(0); //correu bem

}
