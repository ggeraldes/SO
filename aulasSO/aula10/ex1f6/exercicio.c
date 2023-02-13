//exemplo 3
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




typedef struct{
 char tipo;
 int stop;
 pthread_mutex_t *m;
} TDADOS;


void imprime (char letra, int tempo){
    
  for(int i=0;i<tempo;i++){
    printf("%c", letra);
    fflush(stdout);
    sleep(1);
  }
}

void *tarefa(void * dados){
  TDADOS *pdados = (TDADOS * ) dados;
    while(pdados->stop){
        srand(time(0));
        int t=rand() % (1-5);
        imprime('.', t);
        //inicio da critica
        pthread_mutex_lock (pdados->m);
        imprime(pdados->tipo,3);
        pthread_mutex_unlock (pdados->m);

    }
  pthread_exit(NULL);
}

int  main(){
    int i;
    pthread_t t[2];
    pthread_mutex_t  mutex;  
    pthread_mutex_init(&mutex, NULL); //inicializar
    TDADOS valores[2];
    char buffer[100];
    
    valores[0].stop=1;
    valores[0].tipo = 'A';
    valores[0].m = &mutex;

    valores[1].stop=1;
    valores[1].tipo = 'B';
    valores[1].m = &mutex;

    for (i=0;i<2;i++)
        if (pthread_create (&t[i],NULL,&tarefa,&valores[i])!=0)
            return -1; 

    while(strcmp(buffer,"sair")!=0)
        scanf("%s",buffer);

        valores[0].stop = 0;
        valores[1].stop = 0;

    for (i=0;i<2;i++)
       if (pthread_join(t[i], NULL)!=0)
           return -1;

    pthread_mutex_destroy(&mutex);

    return 0;
      
     
}