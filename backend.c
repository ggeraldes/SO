//===================== BACKEND =====================//
#include "header.h"

int verificaUtilizadores(int contadorClientes){

	
		if(contadorClientes==-1)
			return -1;
		else if(contadorClientes==0)
			return -1;
		else
			return 1;
}

int verificaItems (int contadorItems){
		if(contadorItems<=0)
			return -1;
		else
			return 1;
}

void *imprimePromotor(void *a){
	killThread *ta = (killThread*) a;
	/*-----------------PROMOTOR-------------------*/
				int p_b[2];
				int size, pid;
				int estado;	

				pipe(p_b);
				char  res[100];

				int f = fork();
				if(f == 0){
					//filho corre o promotor
					pid=getpid();
					close(p_b[0]);
					close(1);
					dup(p_b[1]);

					execlp("./promotor_oficial", "promotor_oficial", NULL);
					exit(-1);
						
				}
					
				//pai continua a correr
				close(p_b[1]);
				
				while((size = read(p_b[0], res, 99))>0 && ta->kill!=1){
					res[size] = '\0';
					if(strcmp(res,"\n")!=0)
						printf("Promotor: %s\n", res);

				}
				close(p_b[0]);
				

				wait(&estado);

			
	
    pthread_exit(NULL);
}

void *imprimePromotorBF(void *a){
	killThread *ta = (killThread*) a;
	/*-----------------PROMOTOR-------------------*/
				int p_b[2];
				int size, pid;
				int estado;	

				pipe(p_b);
				char  res[100];

				int f = fork();
				if(f == 0){
					//filho corre o promotor
					pid=getpid();
					close(p_b[0]);
					close(1);
					dup(p_b[1]);

					execlp("./black_friday", "black_friday", NULL);
					exit(-1);
						
				}
					
				//pai continua a correr
				close(p_b[1]);
				
				while((size = read(p_b[0], res, 99))>0 && ta->kill!=2){
					res[size] = '\0';
					if(strcmp(res,"\n")!=0)
						printf("Promotor BF: %s\n", res);

				}
				close(p_b[0]);
				

				wait(&estado);

			
	
    pthread_exit(NULL);
}

void handler_sigalarm(int s,siginfo_t *t, void *v){
                    unlink (BACKENDFIFO);
					
                    printf("\nAdeus\n");
                    exit (1);

}


int main(){

	struct sigaction sa;
    sa.sa_sigaction=handler_sigalarm;
    sigaction(SIGINT, &sa, NULL);

	/*------------thread promotor---------------
	killThread a;
	pthread_t promotor;
	pthread_t promotorBF;
	a.kill=0;
	if(pthread_create(&promotor,NULL,&imprimePromotor, &a)!=0)
		printf("promotor nao da!");
	if(pthread_create(&promotorBF,NULL,&imprimePromotorBF, &a)!=0)
		printf("promotor BF nao da!");
*/
	/*------------------------------------------*/

	int op;

	int i=0, count=0;
	char c;
	char palavra[20];
	int contadorClientes=0, contadorItens=0;
	contadorClientes = loadUsersFile("./FUSERS.txt");
	//Cliente clientes[contadorClientes];

	msgBF comunicacao;
	Cliente clientesOnline[NMAXUSERS];
	int flagNMax=0;
	strcpy(clientesOnline[0].nome, "a");

	/* GUARDAR DADOS DO FICHEIRO DE UTILIZADORES NA ESTRUTURA
	if (verificaUtilizadores(contadorClientes)==1){

		i=0;
		count=3;
		FILE *f = fopen("FUSERS.txt", "r");

				
		while((c = fgetc(f))!= EOF){
			ungetc (c,f);
			if(f){
				while(fscanf(f, "%19s", palavra)==1){
					if(i>=contadorClientes)
						break;
					if(count==3)
						strcpy(clientes[i].nome, palavra);
					else if(count==2)
						strcpy(clientes[i].pw, palavra);
					else if(count==1){
						clientes[i].saldo=atoi(palavra);
						i++;
					}
					if (count!=1)
						count--;
					else
						count=3;			
				}
					
			}
		}
		fclose(f);
	}
	*/
	/*----------------------------------------------------------------------------*/
	
	/* GUARDAR DADOS DO FICHEIRO DE ITENS NA ESTRUTURA*/
	i=0;
	count=5;
	c=0;
	int ch=0;
	FILE *f2 = fopen("FITEMS.txt", "r");
	while(!feof(f2)){
  		ch = fgetc(f2);
  		if(ch == '\n')
		contadorItens++;
	}
	fclose(f2);
	if (contadorItens>0)
		contadorItens+=1;

	Item itens[contadorItens];
	if(verificaItems(contadorItens)==1){
		FILE *f3 = fopen("FITEMS.txt", "r");

		while((c = fgetc(f3))!= EOF){
			ungetc (c,f3);
			if(f2){
				while(fscanf(f3, "%19s", palavra)==1){
					if(i>=contadorItens)
						break;
					if(count==5)
						itens[i].ID=atoi(palavra);
					else if(count==4)
						strcpy(itens[i].nome, palavra);
					else if(count==3)
						strcpy(itens[i].categoria, palavra);
					else if(count==2)
						itens[i].valbase=atoi(palavra);
					else if(count==1)
						itens[i].valcp=atoi(palavra);
					if (count!=1)
						count--;
					else{
						count=5;
						i++;
					}		
				}				
			}
		}
		fclose(f3);
	}
	/*----------------------------------------------------------------------------*/

	/*VALIDACAO DE SESSÕES DO FRONTEND*/

	int fd, n, fdr;
	int sel, sel1;
	char fifo[40];
	fd_set fds;

	if(access(BACKENDFIFO, F_OK)!=0){//verifica se o ficheiro "BACKENDFIFO" existe, SE NAO EXISTE CRIA O FIFO
    	mkfifo(BACKENDFIFO, 0600);
    	printf("Criei o fifo\n");
    }
	
	fd = open(BACKENDFIFO, O_RDWR);  //bloqueia - se não houver clientes
    	printf("Abri o fifo '%s'\n", BACKENDFIFO); //acrescentar ao nome do fifo, o pid para saber a que utilizador enviar a resposta
	do{


		
		printf("\n<COMANDO>\n>");
		fflush(stdout);

		FD_ZERO(&fds);        // limpa o fds
        FD_SET(0, &fds);      // adiciona o stdin
        FD_SET(fd, &fds);     // adiciona o fd

		sel =select(fd+1, &fds, NULL,NULL, NULL);//bloqueia aqui

		if(sel>0 && FD_ISSET(0, &fds)){	
			
			

				char com[50];
				char stcom[50];
				char ndcom[50];

				fgets(com, 49, stdin);

				int i, j, k;
				int spaceflag=0;

				
				for(i=1; com[i]!='\0'; i++){

					if(com[i-1]==' ' && com[i]!=' '){
						spaceflag=1;
						break;
					}
				}
				if(spaceflag == 1){

					for(i=0; com[i]!= ' '; i++){
						stcom[i] = com[i];
					}
					stcom[i] = '\0';

					if(strcmp(stcom, "kick\n")==0){
						for(k=0,j=i+1; com[j]!='\0'; j++, k++){
							ndcom[k] = com[j];
						}
						printf("\nBanir o utilizador %s\n", ndcom);
					}

					else if(strcmp(stcom, "cancel")==0){

						for(k=0,j=i+1; com[j]!='\0'; j++, k++){
							ndcom[k] = com[j];
						}
						
						printf("\nCancelar o promotor %s\n", ndcom);
						//a.kill=1;
					}
					else 
						printf("\nComando invalido!\n");
				}

				else{
					

					if(strcmp(com,"users\n")==0){

						printf("\nLista os utilizadores\n");
						if(contadorClientes==-1)
							printf("Aconteceu um ERRO a abrir o ficheiro 'FUSERS.txt'");
						
						else if(contadorClientes==0)
							printf("Não há clientes");

						else{
							//int res=0;
							int g;
							for(g=0; g<NMAXUSERS;g++){

								if(strcmp(clientesOnline[g].nome, "a")==0)
								break; 
								else
								printf("Utilizador: %s, Pass: %s, Saldo: %d\n", clientesOnline[g].nome, clientesOnline[g].pw, clientesOnline[g].saldo);
								/*
								res = updateUserBalance(clientes[g].nome, clientes[g].saldo-1);
								
								if(res==-1)
								printf("\n[ERROR]-updateUserBalance\n");
								else if(res==0)
								printf("\nUtilizador não encontrado\n");
								else
								clientes[g].saldo-=1;
								*/
							}
							if(g==0)
								printf("Sem utilizadores online!\n");
							/*
							res=saveUsersFile("FUSERS.txt");
							if(res==0)
							printf("\nUtilizador guardados com sucesso\n");
							else 
							printf("\n[ERROR]-saveUsersFile\n");

							*/

						}

					}
					else if(strcmp(com,"list\n")==0){

						printf("\nLista os itens a venda\n");
						if(verificaItems(contadorItens)==1){
							for(int g=0; g<contadorItens;g++)
								printf("\nID: %d, Nome: %s, Categ.: %s, VB: %d, VC: %d", itens[g].ID, itens[g].nome, itens[g].categoria, itens[g].valbase, itens[g].valcp);

						printf("\n");
						}
						else
							printf("\nNão foram encontrados itens!");

					}
					else if(strcmp(com,"prom\n")==0){
						printf("\nLista os promotores\n");
					}
					else if(strcmp(com,"reprom\n")==0){
						printf("\nAtualiza os promotores\n");
					}
					else if(strcmp(com,"close\n")==0){
						printf("\nA plataforma vai encerrar\n");
						/*a.kill=1;
						pthread_join(promotor, NULL);*/
						exit(-1);
					}
					else
						printf("\nComando invalido!\n");
				}

			


			
			
		}
		else if(sel>0 && FD_ISSET(fd, &fds)){

			
				if((n= read(fd, &comunicacao, sizeof(msgBF)))>0){ // HA DADOS.... NAO BLOQUEIA
					printf("Recebi...%d %s %d (%d bytes)\n", comunicacao.codMsg, comunicacao.arg1, comunicacao.pid, n);

					if(comunicacao.codMsg==0){//iniciar sessão

						int estaOnline=0;

						for(int i=0; i<NMAXUSERS; i++){
							if(strcmp(clientesOnline[i].nome,comunicacao.arg1)==0 && strcmp(clientesOnline[i].pw,comunicacao.arg2)==0){
								estaOnline=1;
								break;
							}
						}
						if(estaOnline==0 && flagNMax==0){
						//verifica na estrutura se existe
						
							if(isUserValid(comunicacao.arg1, comunicacao.arg2)==1){
									strcpy(comunicacao.arg3, "1");
									printf("%s",comunicacao.arg3);

									//meter utilizador na estrutura online
									for(int i=0;i<NMAXUSERS;i++){
										if(strcmp(clientesOnline[i].nome, "a")==0){

											strcpy(clientesOnline[i].nome, comunicacao.arg1);
											strcpy(clientesOnline[i].pw, comunicacao.arg2);
											clientesOnline[i].saldo=getUserBalance(comunicacao.arg1);
											clientesOnline[i].pid=comunicacao.pid;
											if(i<NMAXUSERS-1){
												strcpy(clientesOnline[i+1].nome, "a");
											}
											else if (i==NMAXUSERS-1)
												flagNMax=1;
											break;
										}
									}
									sprintf(comunicacao.resposta, "Bem vindo!");
								}else
									sprintf(comunicacao.resposta, "Utilizador não existe!");

							//enviar resposta
							sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
							fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
							if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
								close(fdr);
								printf("Enviei...%d %s %s %d %s (%d bytes)\n", comunicacao.codMsg, comunicacao.arg1, comunicacao.arg2, comunicacao.pid, comunicacao.arg3, n);
							}
							else
								printf("[ERRO] - ao enviar dados a utilizador");
						}else{
							if(estaOnline==1)
								sprintf(comunicacao.resposta, "Utilizador já se encntra online!");
							else if(flagNMax==1)
								sprintf(comunicacao.resposta, "O número máximo de utilizadores foi atingido!");
							//enviar resposta
							sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
							fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
							if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
								close(fdr);
								printf("Enviei...%d %s %s %d %s (%d bytes)\n", comunicacao.codMsg, comunicacao.arg1, comunicacao.arg2, comunicacao.pid, comunicacao.arg3, n);
							}
							else
								printf("[ERRO] - ao enviar dados a utilizador");
						}

					}

					else if(comunicacao.codMsg==1){

					}
					else if(comunicacao.codMsg==2){

					}
					else if(comunicacao.codMsg==3){
						

						
					}
					else if(comunicacao.codMsg==4){

						strcpy(comunicacao.arg1, clientesOnline[0].nome);
						strcpy(comunicacao.arg2, clientesOnline[1].nome);

						//enviar resposta
						sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
						fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
						if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
							close(fdr);
							printf("Enviei... (%d bytes)\n", n);
						}
						else
							printf("[ERRO] - ao enviar dados a utilizador");

					}
					else if(comunicacao.codMsg==5){

					}
					else if(comunicacao.codMsg==6){

						

					}
					else if(comunicacao.codMsg==7){

						time_t t = time(NULL);
						struct tm tm = *localtime(&t);
						char teste[50];
						
						sprintf(comunicacao.resposta, "%d-%d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);	//data
						//strcat(comunicacao.arg1, teste);	//hora


						//enviar resposta
						sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
						fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
						if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
							close(fdr);
							printf("Enviei a data e hora para %d(%d bytes)\n", comunicacao.pid, n);
						}else
							printf("[ERRO] - ao enviar dados a utilizador");

					}
					else if(comunicacao.codMsg==8){

					}
					else if(comunicacao.codMsg==9){

					}
					else if(comunicacao.codMsg==10){

						int y, saldo;
						for(y=0; y<NMAXUSERS; y++){
							if(clientesOnline[y].pid==comunicacao.pid)
								break;
							
						}
						saldo=getUserBalance(clientesOnline[y].nome);

						if(saldo!=-1){
							sprintf(comunicacao.arg1,"%d", saldo);
							printf("%s", comunicacao.arg2);
							//enviar resposta
							sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
							fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
							if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
								close(fdr);
								printf("Enviei a quantidade de dinheiro %d(%d bytes)\n", comunicacao.pid, n);
							}else
								printf("[ERRO] - ao enviar dados a utilizador");
						}
						else{
							strcpy(comunicacao.arg1, "[ERRO] - Ao procurar o saldo");
							//enviar resposta
							sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
							fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
							if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
								close(fdr);
								printf("Enviei a quantidade de dinheiro %d(%d bytes)\n", comunicacao.pid, n);
							}else
								printf("[ERRO] - ao enviar dados a utilizador");
						}



					}
					else if(comunicacao.codMsg==11){

						int y, j;
						for(y=0; y<NMAXUSERS; y++){
							if(clientesOnline[y].pid==comunicacao.pid)
								break;
							
						}
						if(y>=0){
							for (j=y;j<NMAXUSERS; j++){  
								if(strcmp(clientesOnline[i+1].nome, "a")==0)
									break;
								if(j+1<NMAXUSERS){
									strcpy(clientesOnline[j].nome, clientesOnline[j+1].nome);
									strcpy(clientesOnline[j].pw, clientesOnline[j+1].pw);
									clientesOnline[j].saldo=clientesOnline[j+1].saldo;
									clientesOnline[j].pid=clientesOnline[j+1].pid;
								}
								else if(j==NMAXUSERS-1  && flagNMax==1){
									strcpy(clientesOnline[j].nome, "a");
									flagNMax=0;
								}
							} 
						}
						
						else
							printf("[ERRO] - ao desconectar utilizador %s", clientesOnline[i].nome);
						
					}

				}else
					printf("[ERRO] - ao receber dados do utiliziador");
				
			
		}

	}while(1);

	
}
