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


void acorda(int s, siginfo_t *info, void *c) { }

typedef struct{
 int contador;
 pthread_mutex_t *m;
} TDADOS;


void *imprime(void * dados){
  TDADOS *pdados = (TDADOS * ) dados;
  for (int k=0;k<20000000;k++){
       //inicio de uma zona critica
     pthread_mutex_lock(pdados->m);  
          pdados->contador++; // le da memoria , incrementa localmente e grava na memoria
      pthread_mutex_unlock(pdados->m);
       //fim de uma zona critica
        fflush(stdout);
  }    
  //printf("[%d]",pdados->contador);
  pthread_exit(NULL);
}

int  main(){
    int i;
    pthread_t t[20];
    pthread_mutex_t  mutex;  
    pthread_mutex_init(&mutex, NULL); //inicializar
    TDADOS dados;
    dados.m = &mutex;
    dados.contador = 0;
    for (i=0;i<2;i++)
      if (pthread_create (&t[i],NULL,&imprime,&dados)!=0)
      return -1;  
     for (i=0;i<2;i++)
       if (pthread_join(t[i], NULL)!=0)
           return -1;
     pthread_mutex_destroy(&mutex);
     printf("\n[%d]",dados.contador);
     
}