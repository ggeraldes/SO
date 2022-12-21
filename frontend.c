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

	//verificaExistencia utilizador;
	msgBF comunicacao;
	respostaBF respostaB;
	int fd, n, fdr;

	if(access(BACKENDFIFO, F_OK)!=0){//verifica se o ficheiro "BACKENDFIFO" existe, SE NAO EXISTE CRIA O FIFO
		fprintf(stderr, "[ERRO] O backend nao esta a funcionar!\n");
		exit (1);
    } 

	//criar fifo FRONTENDFIFO+pid
	comunicacao.pid=getpid();
	sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
	mkfifo(fifo, 0600);

	//Abrir o fifo do backend
	fd = open(BACKENDFIFO, O_WRONLY);
    printf("Abri o fifo do backend - '%s'\n", BACKENDFIFO);

	//meter dados da sessão na estrutura, para enviar ao backend
	comunicacao.codMsg=0;

	strcpy(comunicacao.arg1, argv[1]);
	strcpy(comunicacao.arg2, argv[2]);
	
	strcpy(comunicacao.arg3, "0");

	//mandar a struct ao backend
	if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
	printf("Enviei...%d %s %d %s (%d bytes)\n", comunicacao.codMsg, comunicacao.arg1, comunicacao.pid, comunicacao.arg3, n);
	}else
		printf("[ERRO] - Ao comunicar com o backend\n");

	//receber resposta de validacao do backend
	fdr= open(fifo, O_RDONLY);
	if((n = read(fdr, &comunicacao, sizeof(msgBF)))>0)
	close(fdr);
	else
		printf("[ERRO] - Ao receber do backend\n");

	
	if(atoi(comunicacao.arg3)==1){
		printf("\n%s",comunicacao.resposta);
	}

	else{
		printf("\n%s\n", comunicacao.resposta);
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
				for(k=0,j=i+1; com[j]!='\n'; j++, k++){
					ndcom[k] = com[j];
				}
				ndcom[k+1]='\0';
				strcpy(comunicacao.arg1, ndcom);
				comunicacao.codMsg=4;

				//mandar a struct ao backend
				if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
				printf("Enviei...%d %s %d (%d bytes)\n", comunicacao.codMsg, comunicacao.arg1, comunicacao.pid, n);
				}
				else
					printf("[ERRO] - Ao comunicar com o backend\n");

				//receber resposta do backend
				fdr= open(fifo, O_RDONLY);
				if((n = read(fdr, &comunicacao, sizeof(msgBF)))>0){
					close(fdr);

					printf("\nListar os itens do utilizador %s\n", ndcom);
					printf("%s\n",  comunicacao.arg1);
					printf("%s",  comunicacao.arg2);
				}
				else
					printf("[ERRO] - Ao receber do backend\n");


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

			comunicacao.codMsg=7;

			//mandar a struct ao backend
			if((n=write(fd, &comunicacao, sizeof(msgBF)))>0)
				printf("Enviei...%d (%d bytes)\n", comunicacao.codMsg, n);
			else
				printf("[ERRO] - Ao comunicar com o backend\n");

			//receber resposta do backend
			fdr= open(fifo, O_RDONLY);
			if((n = read(fdr, &comunicacao, sizeof(msgBF)))>0){
				close(fdr);

				printf("\nHora atual: ");
				printf("%s", comunicacao.resposta);
			}
			else
				printf("[ERRO] - Ao receber do backend\n");
		}

		else if(strcmp(com,"cash\n")==0){

			comunicacao.codMsg=10;

			//mandar a struct ao backend
			if((n=write(fd, &comunicacao, sizeof(msgBF)))>0)
				printf("Enviei...%d (%d bytes)\n", comunicacao.codMsg, n);
			else
				printf("[ERRO] - Ao comunicar com o backend\n");

			//receber resposta do backend
			fdr= open(fifo, O_RDONLY);
			if((n = read(fdr, &comunicacao, sizeof(msgBF)))>0){
				close(fdr);
				printf("\nSaldo atual: ");
				printf("%s €", comunicacao.arg1);
			}
			else
				printf("[ERRO] - Ao receber do backend\n");
		}

		else if(strcmp(com,"exit\n")==0){

			comunicacao.codMsg=11;

			//mandar a struct ao backend
			if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
				printf("Enviei...%d (%d bytes)\n", comunicacao.codMsg, n);
				unlink (fifo);
				exit(-1);
			}
			else
				printf("[ERRO] - Ao comunicar com o backend\n");
		}

		else
			printf("\nComando inválido!\n");
		}

	}while(1);

	

}