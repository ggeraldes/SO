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
#include "users_lib.h"


struct cliente{
	char nome[20];
	char pw[20];	     //password
	int saldo;
};

typedef struct item{
	int ID;
	char nome[20];
	char categoria[20];
	int valbase;         //valor base
	int valcp;           //valor "comprar já"
}Item;

struct leilao{
	Item item;
	bool promocao;
};


#endif //_HEADER_H_
