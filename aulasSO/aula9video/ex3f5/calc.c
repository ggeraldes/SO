#include "util.h"

int main(void) {  

    PEDIDO p;
    int fd, fdr, n, var=0, res;
    char fifo[40], cmd[40];

    fd_set fds;
    struct timeval tempo;

    //criar named pipe (fifo)

    if(access(FIFO_SRV, F_OK)!=0){//verifica se o ficheiro "FIFO_SRV" existe, SE NAO EXISTE CRIA O FIFO

    mkfifo(FIFO_SRV, 0600);
    printf("Criei o fifo\n");
    } 

    fd = open(FIFO_SRV, O_RDONLY);  //bloqueia - se não houver clientes
    printf("Abri o fifo '%s'\n", FIFO_SRV); //acrescentar ao nome do fifo, o pid para saber a que utilizador enviar a resposta
    do {

        printf("COMANDO: "); fflush(stdout); 

        //usar o select para saber onde esta a informacao ....
        FD_ZERO(&fds);        // limpa o fds
        FD_SET(0, &fds);      // adiciona o stdin
        FD_SET(fd, &fds);     // adiciona o fd
        tempo.tv_sec=10;      // da um TIMEOUT
        tempo.tv_usec=0; 
       
        res =select(fd+1, &fds, NULL,NULL, &tempo);//bloqueia aqui

        if(res==0)
            printf("\nNao ha dados\n");
        else if(res>0 && FD_ISSET(0, &fds)){

            //ler comandos do administrador
            
            scanf("%s", cmd); // HA DADOS.... NAO BLOQUEIA
            printf("O administrador pediu o comando '%s'...\n", cmd);
            if(strcmp(cmd, "termina")==0)
                p.op='s';
        }
        else if(res>0 && FD_ISSET(fd, &fds)){

            //ler pedido ...
            n= read(fd, &p, sizeof(PEDIDO)); // HA DADOS.... NAO BLOQUEIA
            printf("\nRecebi...%d %c %d pid=%d (%d bytes)\n", p.num1, p.op, p.num2,p.pid,  n);

            

            //fazer a conta ....
            if(n==sizeof(PEDIDO));
            if(p.op=='+'){
                p.res=p.num1+p.num2;var=1;}
            else if(p.op=='-'){
                p.res=p.num1-p.num2;var=1;}
            else if(p.op=='*'){
                p.res=p.num1*p.num2;var=1;}
            else if(p.op=='/'){
                p.res=p.num1/p.num2;var=1;}
            else if(p.op=='s')
                printf("Adeus!\n\n");
            else
                printf("Operador Incorreto!!\n\n");

            //enviar resposta ....
            sprintf(fifo, FIFO_CLI, p.pid);
            if(var==1){
                fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
                n= write(fdr, &p, sizeof(PEDIDO));
                close(fdr);
                printf("Enviei...%d %c %d = %.2f (%d bytes)\n\n", p.num1, p.op, p.num2, p.res, n);
            }
            var=0;
        }

    }while(p.op!='s'); //s - sair
    close(fd); //fechar FIFO


    //APAGAR FIFO
    unlink(FIFO_SRV);

        
    exit(0); //correu bem

}
