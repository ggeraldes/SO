//===================== HEADER =====================//

#ifndef _HEADER_H_

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
#include "users_lib.h"

#define BACKENDFIFO "BACKENDFIFO"
#define FRONTENDFIFO "CLIENTE%d"
char CLIENTE_FIFO_FINAL[100];

#define NMAXUSERS 20




typedef struct{				//serve para parar a thread do promotor
	int kill;				//kill=0, thread continua; kill=1, thread termina
}killThread;				

typedef struct{				//--------------VALIDAÇÃO DA SESSÃO------------------
	char nome[20];			//caso esteja no ficheiro FUSERS.txt validacao=1, se não validacao=0
	char pwd[20];
	int pid;
	int validacao;
}verificaExistencia;


typedef struct{			   //-------------------COMUNICAÇÃO BACKEND-FRONTEND-------------------------
	int codMsg;            //cod=0(para iniciar sessao), cod=1 (para sell), cod=2 (para list), 
	int pid;			   //cod=3 (para licat), cod=4 (para lisel), cod=5 (para lival), cod=6 (para litime), 
	char arg1[20];		   //cod=7 (para time), cod=8 (para buy), cod=9 (para cash), cod=10 (para add), cod=11 (para exit)
	char arg2[20];	
	char arg3[20];
	char arg4[20];
	char arg5[20];	   	
}msgBF;

typedef struct{			   //-------------------RESPOSTA BACKEND-FRONTEND-------------------------
	char resposta[50];   	
}respostaBF;

typedef struct cliente{
	char nome[20];
	char pw[20];	     //password
	int saldo;
	int pid;
}Cliente;

typedef struct item{
	int ID;
	char nome[20];
	char categoria[20];
	int valbase;         //valor base
	int valcp;           //valor "comprar já"
}Item;

typedef struct leilao{
	Item item;
	bool promocao;
}Leilao;


//---------------hora-------------------
char traco='-';
char pontos=':';
char espaco=' ';







#endif //_HEADER_H_
