//===================== FRONTEND =====================//
#include "header.h"

char fifo[40];
msgBF comunicacao;
int HEARTBEATINTERVAL=HEARTBEAT_INTERVAL;
void handler_sigalarm(int s,siginfo_t *t, void *v){
                    unlink (fifo);
                    printf("\nadeus\n");
                    exit (1);

}

void *heartBeatF(void *a){
	int n;
	int fd=open(BACKENDFIFO, O_WRONLY);
	do{
	comunicacao.codMsg=12;
	if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){

		//printf("Enviei o meu heartbeat!\n");

	}
	else
		printf("[ERRO] - Ao comunicar com o backend\n");
	sleep(HEARTBEATINTERVAL);
	}while(1);

}



int main(int argc, char*argv[]){

	struct sigaction sa;
    sa.sa_sigaction=handler_sigalarm;
    sigaction(SIGINT, &sa, NULL);
	
	if(argc != 3){
		printf("\nPor favor indique o seu nome e password!\n");
		exit(-1);
	}

	
	
	//---------------------verificaExistencia utilizador---------------------------
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
    //printf("Abri o fifo do backend - '%s'\n", BACKENDFIFO);

	//meter dados da sessão na estrutura, para enviar ao backend
	comunicacao.codMsg=0;
	sprintf(comunicacao.mensagem, "%s %s", argv[1], argv[2]);
	
	strcpy(comunicacao.arg3, "0");

	//mandar a struct ao backend
	if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
	//printf("Enviei...%d %s %d %s (%d bytes)\n", comunicacao.codMsg, comunicacao.mensagem, comunicacao.pid, comunicacao.arg3, n);
	}else
		printf("[ERRO] - Ao comunicar com o backend\n");

	//receber resposta de validacao do backend
	fdr= open(fifo, O_RDONLY);
	if((n = read(fdr, &comunicacao, sizeof(msgBF)))>0){
		if(atoi(comunicacao.arg3)==1){
			printf("\n%s",comunicacao.resposta);
		}

		else{
			printf("\n%s\n", comunicacao.resposta);
			unlink(fifo);
			close(fd);
			exit (1);
		} 
	}
	else
		printf("[ERRO] - Ao receber do backend\n");

	close(fdr);
	fdr= open(fifo, O_RDWR);
	/*----------------------HEARTBEAT-----------------------*/
	
	
	
	HEARTBEATINTERVAL=comunicacao.heartbeatTime;
	//printf("!%d!", HEARTBEATINTERVAL);
	 pthread_t heartBeat;
	 if (pthread_create (&heartBeat,NULL,&heartBeatF,NULL)!=0)
        printf("[ERRO AO CRIAR HEARTBEAT!");


	/*------------------------------------------------------*/

	/*------------thread informacoes backend---------------*/
	
	
	/*pthread_t recebeB;

	if(pthread_create(&recebeB,NULL,&receberInfo, NULL)!=0)
		printf("promotor nao irá funcionar!");*/


	/*--------------------------------------------*/


	Leilao temp[NMAXITEMS];

	char com[50];    //comando do utilizador
	char stcom[50];  //primeira parte do comando do utilizador
	char ndcom[50];  //segunda parte do comando do utilizador (utilizada nos comandos licat, lisel, lival, litime, add)
	char rdcom[50];  //terceira parte do comando do utilizador (utilizada no comando buy)
	char prbase[10];
	char prcj[10];
	char dur[10];

	int VARIMPOR=0;
	int space=0;
	int ERROARG=0;
	int a=0;
	int b=0;

	fflush(stdin);

	respostaBF envia;

	fd_set fds;
	int sel;

	do{
		if(VARIMPOR!=1 && VARIMPOR!=2){
			printf("\nComando: ");
			fflush(stdout);
		}

		FD_ZERO(&fds);        // limpa o fds
        FD_SET(0, &fds);      // adiciona o stdin
        FD_SET(fdr, &fds);     // adiciona o fdr

		
		sel =select(fdr+1, &fds, NULL,NULL, NULL);//bloqueia aqui
		
		
		if(sel>0 && FD_ISSET(0, &fds)){

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

					comunicacao.codMsg=4;
					

					space=0;
					ERROARG=0;
					a=0;

					for(int i=0; com[i] != '\n'; i++, a++){
									if(com[i]==' '){
										if(com[i-1]==' '){
											ERROARG=1;
											break;
										}
										space++;
										a=-1;	
									}
									else{
										if(space==1)
											ndcom[a]=com[i];
									}
								}
								ndcom[a]='\0';

					if(space==1 && ERROARG!=1){
						strcpy(comunicacao.mensagem, ndcom);

						//mandar a struct ao backend
						if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
							VARIMPOR=1;
							//printf("Enviei...%d %d (%d bytes)\n", comunicacao.codMsg, comunicacao.pid, n);
							printf("\nLista de itens a venda pelo do utilizador '%s':\n", ndcom);
						}
						else
							printf("[ERRO] - Ao comunicar com o backend\n");
							
							
						
					}
					else
						printf("[ERRO] - ARGUMETOS INVALIDOS\n");


				}

				else if(strcmp(stcom, "licat")==0){

					comunicacao.codMsg=3;
					

					space=0;
					ERROARG=0;
					a=0;

					for(int i=0; com[i] != '\n'; i++, a++){
									if(com[i]==' '){
										if(com[i-1]==' '){
											ERROARG=1;
											break;
										}
										space++;
										a=-1;	
									}
									else{
										if(space==1)
											ndcom[a]=com[i];
									}
								}
								ndcom[a]='\0';

					if(space==1 && ERROARG!=1){
						strcpy(comunicacao.mensagem, ndcom);

						//mandar a struct ao backend
						if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
							VARIMPOR=1;
							//printf("Enviei...%d %d (%d bytes)\n", comunicacao.codMsg, comunicacao.pid, n);
							printf("\nLista de itens da categoria '%s':\n", ndcom);
						}
						else
							printf("[ERRO] - Ao comunicar com o backend\n");

						

						
					}
					else
						printf("[ERRO] - ARGUMETOS INVALIDOS\n");
				}

				else if(strcmp(stcom, "lival")==0){

					comunicacao.codMsg=5;
					

					space=0;
					ERROARG=0;
					a=0;

					for(int i=0; com[i] != '\n'; i++, a++){
									if(com[i]==' '){
										if(com[i-1]==' '){
											ERROARG=1;
											break;
										}
										space++;
										a=-1;	
									}
									else{
										if(space==1)
											ndcom[a]=com[i];
									}
								}
								ndcom[a]='\0';

					if(space==1 && ERROARG!=1 && atoi(ndcom)>0){
						strcpy(comunicacao.mensagem, ndcom);

						//mandar a struct ao backend
						if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
							VARIMPOR=1;
							//printf("Enviei...%d %d (%d bytes)\n", comunicacao.codMsg, comunicacao.pid, n);
							printf("\nLista des itens cujo o valor máx. é %s€:\n", ndcom);
						}
						else
							printf("[ERRO] - Ao comunicar com o backend\n");

							


						
					}
					else
						printf("[ERRO] - ARGUMETOS INVALIDOS\n");
				}

				else if(strcmp(stcom, "litime")==0){
					comunicacao.codMsg=6;
					

					space=0;
					ERROARG=0;
					a=0;

					for(int i=0; com[i] != '\n'; i++, a++){
									if(com[i]==' '){
										if(com[i-1]==' '){
											ERROARG=1;
											break;
										}
										space++;
										a=-1;	
									}
									else{
										if(space==1)
											ndcom[a]=com[i];
									}
								}
								ndcom[a]='\0';

					if(space==1 && ERROARG!=1 && atoi(ndcom)>0){
						strcpy(comunicacao.mensagem, ndcom);

						//mandar a struct ao backend
						if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
							VARIMPOR=1;
							//printf("Enviei...%d %d (%d bytes)\n", comunicacao.codMsg, comunicacao.pid, n);
							printf("\nLista de itens com prazo até %ss:\n", ndcom);
						}
						else
							printf("[ERRO] - Ao comunicar com o backend\n");
		

						
					}
					else
						printf("[ERRO] - ARGUMETOS INVALIDOS\n");
				}

				else if(strcmp(stcom, "add")==0){

					comunicacao.codMsg=10;
					

					space=0;
					ERROARG=0;
					a=0;

					for(int i=0; com[i] != '\n'; i++, a++){
									if(com[i]==' '){
										if(com[i-1]==' '){
											ERROARG=1;
											break;
										}
										space++;
										a=-1;	
									}
									else{
										if(space==1)
											ndcom[a]=com[i];
									}
								}
								ndcom[a]='\0';

					if(space==1 && ERROARG!=1 && atoi(ndcom)>0){
						strcpy(comunicacao.mensagem, ndcom);
						
						//mandar a struct ao backend
						if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
							VARIMPOR=2;
							//printf("Enviei...%d %d (%d bytes)\n", comunicacao.codMsg, comunicacao.pid, n);
						}
						else
							printf("[ERRO] - Ao comunicar com o backend\n");

					}	
					else
						printf("[ERRO] - ARGUMETOS INVALIDOS\n");

				}

				else if(strcmp(stcom, "buy")==0){

					comunicacao.codMsg=8;
					

					space=0;
					ERROARG=0;
					b=0;
					a=0;

					for(int i=0; com[i] != '\n'; i++, a++){
									if(com[i]==' '){
										if(com[i-1]==' '){
											ERROARG=1;
											break;
										}
										space++;
										b=a;
										a=-1;	
									}
									else{
										if(space==1)
											ndcom[a]=com[i];
										if(space==2){
											ndcom[b]='\0';
											rdcom[a]=com[i];	
										}
									}
								}rdcom[a]='\0';
								

					if(space==2 && ERROARG!=1 && atoi(rdcom)>0 && atoi(ndcom)>0){
						sprintf(comunicacao.mensagem, "%s %s %s", ndcom, rdcom, argv[1]);
						//mandar a struct ao backend
						if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
							VARIMPOR=2;
							//printf("Enviei...%d (%d bytes)\n", comunicacao.codMsg, n);
						}
						else
							printf("[ERRO] - Ao comunicar com o backend\n");

					}else
						printf("[ERRO] - ARGUMETOS INVALIDOS\n");
					
					
				}

				else if(strcmp(stcom, "sell")==0){

					comunicacao.codMsg=1;
					

					space=0;
					ERROARG=0;
					b=0;
					a=0;

					for(int i=0; com[i] != '\n'; i++, a++){
									if(com[i]==' '){
										if(com[i-1]==' '){
											ERROARG=1;
											break;
										}
										space++;
										b=a;
										a=-1;	
									}
									else{
										if(space==1)
											ndcom[a]=com[i];
										if(space==2){
											ndcom[b]='\0';
											rdcom[a]=com[i];	
										}
										if(space==3){
											rdcom[b]='\0';
											prbase[a]=com[i];
										}
										if(space==4){
											prbase[b]='\0';
											prcj[a]=com[i];
										}
										if(space==5){
											prcj[b]='\0';
											dur[a]=com[i];
										}
									}
								}dur[a]='\0';
								
					
					if(space==5 && ERROARG!=1 && atoi(prbase)>0 && atoi(prcj)>0 && atoi(prbase)>0 && atoi(prbase)<atoi(prcj)){
						sprintf(comunicacao.mensagem, "%s %s %s %s %s %s", ndcom, rdcom, prbase, prcj, dur, argv[1]);
						//printf("\nVender o item %s da categoria %s, com um preco base de %s, disponivel ja por %s durante %s segundos\n", comunicacao.mensagem, comunicacao.arg2, comunicacao.arg3, comunicacao.arg4, comunicacao.arg5);

						//mandar a struct ao backend
						if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
							VARIMPOR=2;
							//printf("Enviei...%d (%d bytes)\n", comunicacao.codMsg, n);
						}
						else
							printf("[ERRO] - Ao comunicar com o backend\n");

					}else
						printf("[ERRO] - ARGUMETOS INVALIDOS\n");

				}

				else
					printf("\nComando inválido!\n");

			}

			else if(spaceflag==0){
			if(strcmp(com,"list\n")==0){
				comunicacao.codMsg=2;
				
				//mandar a struct ao backend
					if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
						VARIMPOR=1;
						//printf("Enviei...%d (%d bytes)\n", comunicacao.codMsg, n);
						printf("Lista de itens em Leilao:\n");
					}
					else
						printf("[ERRO] - Ao comunicar com o backend\n");
					



				
			}

			else if(strcmp(com,"time\n")==0){

				comunicacao.codMsg=7;
				
				//mandar a struct ao backend
				if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
					VARIMPOR=2;
					//printf("Enviei...%d (%d bytes)\n", comunicacao.codMsg, n);
				}
				else
					printf("[ERRO] - Ao comunicar com o backend\n");

			}

			else if(strcmp(com,"cash\n")==0){

				comunicacao.codMsg=9;
				

				strcpy(comunicacao.mensagem, argv[1]);
				//mandar a struct ao backend
				if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
					VARIMPOR=2;
					//printf("Enviei...%d (%d bytes)\n", comunicacao.codMsg, n);
				}
				else
					printf("[ERRO] - Ao comunicar com o backend\n");

			}

			else if(strcmp(com,"exit\n")==0){

				comunicacao.codMsg=11;

				//mandar a struct ao backend
				if((n=write(fd, &comunicacao, sizeof(msgBF)))>0){
					//printf("Enviei...%d (%d bytes)\n", comunicacao.codMsg, n);
					close(fdr);
					unlink (fifo);
					exit(-1);
				}
				else
					printf("[ERRO] - Ao comunicar com o backend\n");
			}

			else{
				VARIMPOR=0;
				printf("\nComando inválido!\n");
			}
			}
		}
		else if(sel>0 && FD_ISSET(fdr, &fds)){	
			if(VARIMPOR==0){
				//receber resposta do backend
						
						if((n = read(fdr, &envia, sizeof(respostaBF)))>0){
							
							printf("\n%s\n",  envia.resposta);
							if(envia.kill==1){
								close(fdr);
								unlink (fifo);
								exit(-1);
							}
						}
						else
							printf("[ERRO] - Ao receber do backend\n");
				
			}

			else if(VARIMPOR==1){
						
						
						if((n = read(fdr, &temp, sizeof(temp)))>0){
							int g=0, valorCj=0;
							for(g=0; strcmp(temp[g].nome, "a")!=0 && g<NMAXITEMS;g++){
	
								if(temp[g].prom!=0)
									valorCj=temp[g].valcp-temp[g].valcp*temp[g].prom/100;
								else
									valorCj=temp[g].valcp;

									printf("\nID: %d, Nome: %s, Categ.: %s, VB: %d€, VC: %d€, dur: %ds, vend.: %s", temp[g].ID, temp[g].nome, temp[g].categoria, temp[g].valbase, valorCj, temp[g].duracao, temp[g].nomeVend);
									if(strcmp(temp[g].nomeLic, "-")!=0)
										printf(", ult.Lic.: %s.", temp[g].nomeLic);
									else
										printf(", ainda sem licitacoes.");
							
									if(temp[g].prom!=0)
										printf(" Em prom: %d%%, dur.:%d", temp[g].prom, temp[g].promDur);
								}
								if(g==0)
									printf("Lista vazia\n");
							strcpy(temp[0].nome, "a");
						}
						else
							printf("[ERRO] - Ao receber do backend\n");
			}

			else if(VARIMPOR==2){
				//receber resposta do backend
						
						if((n = read(fdr, &comunicacao, sizeof(msgBF)))>0){
							
							printf("%s\n",  comunicacao.resposta);
						}
						else
							printf("[ERRO] - Ao receber do backend\n");
			}
			

			VARIMPOR=0;

		}
			

	}while(1);

	

}