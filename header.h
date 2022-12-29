//===================== HEADER =====================//

#ifndef _HEADER_H_BACKENDFIFO

#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include "users_lib.h"

#define BACKENDFIFO "BACKENDFIFO"
#define BACKENDFIFOG "BACKENDFIFOG"
#define FRONTENDFIFO "CLIENTE%d"
char CLIENTE_FIFO_FINAL[100];

#define NMAXUSERS 20
#define NMAXPROMOS 10
#define NMAXITEMS 30




typedef struct{				//serve para parar a thread do promotor
	int kill;				//kill=0, thread continua; kill=1, thread termina
	char nomePromotor[30];
}killThread;				



typedef struct{			   		//-------------------COMUNICAÇÃO BACKEND-FRONTEND-------------------------
	int codMsg;            		//cod=0(para iniciar sessao), cod=1 (para sell), cod=2 (para list), 
	int pid;			   		//cod=3 (para licat), cod=4 (para lisel), cod=5 (para lival), cod=6 (para litime), 
	char mensagem[200];		    //cod=7 (para time), cod=8 (para buy), cod=9 (para cash), cod=10 (para add), cod=11 (para exit)
	char arg3[20];				// valida a sessao
	char resposta[100];	   	
}msgBF;

typedef struct{			   //-------------------RESPOSTA BACKEND-FRONTEND-------------------------
	char resposta[200]; 
	int kill;    //se tiver =1 os frontend fecham 	
}respostaBF;


typedef struct cliente{
	char nome[20];
	char pw[20];	     //password
	int pid;
}Cliente;

typedef struct leilao{
	int ID;
	char nome[20];
	char categoria[20];
	int valbase;         //valor base
	int valcp;           //valor "comprar já"
	int duracao;
	char nomeVend[20];
	char nomeLic[20];
	//caso haja promocoes
	int prom;
	int promDur;
	int pidProm;
}Leilao;

typedef struct promotor{
	char ficheiro[30];
	int pidP;
	//pthread_mutex_t *m;
}Promotor;









#endif //_HEADER_H_
