#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>

typedef struct {
    char letra;
    int continua;
}TDATA;

void *mostra(void *dados){ //tarefa #1 --> THREAD ADICIONAL
    int *res;
    TDATA *pdata;
    pdata =(TDATA *) dados;

    do{

        printf("%c", pdata->letra); fflush(stdout); sleep(1);
    }while(pdata->continua);

    res= (int *) malloc(sizeof(int));
    *res = 123;
        
        pthread_exit((void *) res);

}


int  main(){

    char str[40];
    int *estado;
    pthread_t tarefa;
    TDATA tinfo;

    printf("Sou o processo %d...", getpid());

    do {
        tinfo.letra='x';
        tinfo.continua = 1;
        pthread_create(&tarefa, NULL, mostra, &tinfo);

        //tarefa #0
        printf("\nCOMANDO:\n");
        scanf("%s", str);
        printf("Recebi o comando '%s'...\n", str);

    }while(strcmp(str,"sair")!=0);
    printf("vou pedir a thread um para terminar!");
    tinfo.continua=0;
    pthread_join(tarefa, (void*) &estado); //espera pelo fim da thread 1
    printf("\nterminou! %d", *estado);

    exit(0);
}