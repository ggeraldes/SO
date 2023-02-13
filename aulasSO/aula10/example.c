//exemplo 2
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


void acorda(int s, siginfo_t *info, void *c) {

}

typedef struct{
 int a;
 int b;
 int stop;
} TDADOS;


void *imprime(void *dados){
    TDADOS *pdados = (TDADOS*) dados;
    int xpto;
   do{
        printf("Ola da thread  [%d] [%d]\n",pdados->a,pdados->b);    
        sleep(5);
        //scanf("%d",&xpto);   //fica aqui
    }while(pdados->stop);
    pthread_exit(NULL);
}

int  main(){
    struct sigaction act;
      act.sa_sigaction = acorda;
      act.sa_flags = SA_SIGINFO;
      sigaction(SIGUSR1, &act, NULL);
   
    int i;
    pthread_t a;
    TDADOS val;
    val.a = 10;
    val.b = 20;
    val.stop = 1;
    if (pthread_create (&a,NULL,&imprime,&val)!=0)
        return 0;
    scanf("%d",&i);
    val.stop = 0;
    //pthread_kill(a, SIGUSR1);
    pthread_join(a, NULL);
}