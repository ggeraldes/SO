//===================== FRONTEND =====================//
#include "header.h"

char fifo[40];

void handler_sigalarm(int s,siginfo_t *t, void *v){
                    unlink (fifo);
                    printf("\nadeus\n");
                    exit (1);

}

int main(int argc, char*argv[]){

	struct sigaction sa;
    sa.sa_sigaction=handler_sigalarm;
    sigaction(SIGINT, &sa, NULL);
	
	if(argc != 3){
		printf("\nPor favor indique o seu nome e a sua password!\n");
		exit(-1);
	}

	verificaExistencia utilizador;
	int fd, n, fdr;

	if(access(BACKENDFIFO, F_OK)!=0){//verifica se o ficheiro "BACKENDFIFO" existe, SE NAO EXISTE CRIA O FIFO
		fprintf(stderr, "[ERRO] O backend nao esta a funcionar!\n");
		exit (1);
    } 

	//criar fifo FRONTENDFIFO+pid
	utilizador.pid=getpid();
	sprintf(fifo, FRONTENDFIFO, utilizador.pid);
	mkfifo(fifo, 0600);

	//Abrir o fifo do backend
	fd = open(BACKENDFIFO, O_WRONLY);
    printf("Abri o fifo do backend - '%s'\n", BACKENDFIFO);

	//meter dados da sessão na estrutura, para enviar ao backend
	strcpy(utilizador.nome, argv[1]);
	strcpy(utilizador.pwd, argv[2]);
	utilizador.validacao=0;

	//mandar a struct ao backend
	n=write(fd, &utilizador, sizeof(verificaExistencia));
	printf("Enviei...%s %s %d (%d bytes)\n", utilizador.nome, utilizador.pwd, utilizador.pid, n);

	//receber resposta de validacao do backend
	fdr= open(fifo, O_RDONLY);
	n = read(fdr, &utilizador, sizeof(verificaExistencia));
	close(fdr);

	
	
	if(utilizador.validacao==1)
		printf("\nBem vindo!");
	else{
		printf("\nUtilizador não encontrado!\n");
		unlink(fifo);
		close(fd);
		exit (1);
	} 

	char com[50];    //comando do utilizador
	char stcom[50];  //primeira parte do comando do utilizador
	char ndcom[50];  //segunda parte do comando do utilizador (utilizada nos comandos licat, lisel, lival, litime, add)
	char rdcom[50];  //terceira parte do comando do utilizador (utilizada no comando buy)
	char prbase[10];
	char prcj[10];
	char dur[10];
	fflush(stdin);

	do{
		printf("\nComando: ");
		fgets(com, 49, stdin);
		int i, j, k;
		int spaceflag=0;

		for(i=1; com[i]!='\0'; i++){
			if(com[i-1]==' ' && com[i]!=' '){
				spaceflag=1;
				break;
			}
		}

		if(spaceflag==1){
			for(i=0; com[i]!= ' '; i++){
				stcom[i] = com[i];
			}
			stcom[i] = '\0';

			if(strcmp(stcom, "lisel")==0){
				for(k=0,j=i+1; com[j]!='\0'; j++, k++){
					ndcom[k] = com[j];
				}
				printf("\nListar os itens do utilizador %s\n", ndcom);
			}

			else if(strcmp(stcom, "licat")==0){
				for(k=0,j=i+1; com[j]!='\0'; j++, k++){
					ndcom[k] = com[j];
				}
				printf("\nListar os itens da categoria %s\n", ndcom);
			}

			else if(strcmp(stcom, "lival")==0){
				for(k=0,j=i+1; com[j]!='\0'; j++, k++){
					ndcom[k] = com[j];
				}
				int valmax = atoi(ndcom);
				printf("\nListar os itens da que custam até %d\n", valmax);
			}

			else if(strcmp(stcom, "litime")==0){
				for(k=0,j=i+1; com[j]!='\0'; j++, k++){
					ndcom[k] = com[j];
				}
				int valtime = atoi(ndcom);
				printf("\nListar os itens com pazo até %d segundos\n", valtime);
			}

			else if(strcmp(stcom, "add")==0){
				for(k=0,j=i+1; com[j]!='\0'; j++, k++){
					ndcom[k] = com[j];
				}
				int valtime = atoi(ndcom);
				printf("\nAdicionou %d ao saldo atual\n", valtime);
			}

			else if(strcmp(stcom, "buy")==0){
				for(k=0,j=i+1; com[j]!=' '; j++, k++){
					ndcom[k] = com[j];
				}
				int id = atoi(ndcom);
				for(k=0,j+=1; com[j]!='\0'; j++, k++){
					rdcom[k] = com[j];
				}
				int valof = atoi(rdcom);
				printf("\nComprar o item com id %d que custa %d\n", id, valof);
			}

			else if(strcmp(stcom, "sell")==0){
				for(k=0,j=i+1; com[j]!=' '; j++, k++){
					ndcom[k] = com[j];
				}

				for(k=0,j+=1; com[j]!=' '; j++, k++){
					rdcom[k] = com[j];
				}

				for(k=0,j+=1; com[j]!=' '; j++, k++){
					prbase[k] = com[j];
				}

				int prb = atoi(prbase);
				for(k=0,j+=1; com[j]!=' '; j++, k++){
					prcj[k] = com[j];
				}

				int prc = atoi(prcj);
				for(k=0,j+=1; com[j]!='\0'; j++, k++){
					dur[k] = com[j];
				}

				int dtemp = atoi(dur);
				printf("\nVender o item %s da categoria %s, com um preco base de %d, disponivel ja por %d durante %d segundos\n", ndcom, rdcom, prb, prc, dtemp);
			}

			else
				printf("\nComando inválido!\n");

		}

		else if(spaceflag==0){
		if(strcmp(com,"list\n")==0){
			printf("\nLista os itens\n");
		}

		else if(strcmp(com,"time\n")==0){
			printf("\nHora atual: \n");
		}

		else if(strcmp(com,"cash\n")==0){
			printf("\nSaldo atual: \n");
		}

		else if(strcmp(com,"exit\n")==0){
			exit(-1);
		}

		else
			printf("\nComando inválido!\n");
		}

	}while(1);

	

}