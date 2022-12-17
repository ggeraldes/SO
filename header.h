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
#include "users_lib.h"

#define BACKENDFIFO "BACKENDFIFO"
#define FRONTENDFIFO "CLIENTE%d"
char CLIENTE_FIFO_FINAL[100];

typedef struct{
	char nome[20];
	char pwd[20];
	int pid;
	int validacao;
}verificaExistencia;


typedef struct{
	pid_t pid;
	//comando;
}msgFtoB;

typedef struct{
	char res[100];
}msgBtoF;


typedef struct cliente{
	char nome[20];
	char pw[20];	     //password
	int saldo;
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




verificaExistencia utilizador;
int fd, n, fdr;
int sel, sel1;
char fifo[40];
fd_set fds;
void recebeEnvia(int contadorClientes, Cliente *clientes){
	 n= read(fd, &utilizador, sizeof(verificaExistencia)); // HA DADOS.... NAO BLOQUEIA
            printf("\nRecebi...%s, %s, %d, %d (%d bytes)\n", utilizador.nome, utilizador.pwd, utilizador.pid, utilizador.validacao, n);

			//verifica na estrutura se existe
			for(int j=0; j<contadorClientes; j++){

				if(strcmp(clientes[j].nome, utilizador.nome)==0 && strcmp(clientes[j].pw, utilizador.pwd)==0){
					utilizador.validacao=1;
					break;
				}

			}

			//enviar resposta
			sprintf(fifo, FRONTENDFIFO, utilizador.pid);
			fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
			n= write(fdr, &utilizador, sizeof(verificaExistencia));
			close(fdr);
			printf("Enviei...%s, %s, %d, %d (%d bytes)\n", utilizador.nome, utilizador.pwd, utilizador.pid, utilizador.validacao, n);
}

#endif //_HEADER_H_
