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
#define FRONTENDFIFO "CLIENTE%d"

#define NMAXUSERS 2
#define NMAXPROMOS 10
#define NMAXITEMS 10

#define HEARTBEAT_INTERVAL 5 // intervalo em segundos para verificar utilizadores ativos
#define INACTIVE_TIMEOUT 7 // tempo em segundos para considerar utilizador inativo		



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
	time_t last_heartbeat;
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
	int p_b[2];
	int pidP;
	//int kill;
	pthread_mutex_t *m;
	struct promotor *next;
	
}Promotor;









#endif //_HEADER_H_
