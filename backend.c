//===================== BACKEND =====================//
#include "header.h"

/*--------Variaveis Globais-------*/
int flagNMaxI=0;
int flagNMaxU=0;
char fifo[40];
char *fusers;
//necessário pela impossibilidade mais do que uma struct para thread
Cliente clientesOnline[NMAXUSERS]; 
respostaBF envia;
Leilao items[NMAXITEMS];
/*--------------------------------*/

void enviaMensagemU( char resposta[]){

	int fdr, n;
	strcpy(envia.resposta, resposta);
	if(strcmp(envia.resposta, "A plataforma vai encerrar")==0)
		envia.kill=1;

	for(int i=0; i<NMAXUSERS && strcmp(clientesOnline[i].nome, "a")!=0; i++){
		sprintf(fifo, FRONTENDFIFO, clientesOnline[i].pid);
		fdr = open(fifo, O_RDWR);  //bloqueia - se não houver clientes
			if((n= write(fdr, &envia, sizeof(respostaBF)))>0){
				close(fdr);
				printf("Enviei... (%d bytes)\n", n);
			}
			else
				printf("[ERRO] - ao enviar dados a utilizador");
			
	}

}


void *check_active_users(void *a) {
  // Verifica os utilizadores ativos e remove os inativos
  do{
		for (int i = 0; i < NMAXUSERS && strcmp(clientesOnline[i].nome, "a")!=0; i++) {
			time_t now = time(NULL); // obtém a hora atual
			if (now - clientesOnline[i].last_heartbeat > INACTIVE_TIMEOUT) {
				flagNMaxU=0;
			// utilizadore inativo, remove da struct
				for(int g=i; g < NMAXUSERS && strcmp(clientesOnline[g].nome, "a")!=0; g++){
					if(g<NMAXUSERS-1)
						clientesOnline[g] = clientesOnline[g+1];
					if(g==NMAXUSERS-1){
						strcpy(clientesOnline[g].nome,"a");
					}
				}
				i--;
			}
		}
  }while(1);
}

int verificaItems (int contadorItems){
		if(contadorItems<=0)
			return -1;
		else
			return 1;
}

void sig_handler(int sig) {
  exit (1);
}
void *imprimePromotor(void *a){
	Promotor *ta = (Promotor*) a;

	/*-----------------PROMOTOR-------------------*/
				char cat[20], prom[10], dur[10];
				char promocao[200];
				int p_b[2];
				int size;
				int estado;	
				int isto=0;
				pipe(p_b);
				char  res[100];
				struct sigaction action;

				int f = fork();
				
				if(f == 0){
					//filho corre o promotor

					 // Configurar a estrutura sigaction para especificar o manipulador de sinais
					action.sa_handler = sig_handler;
					sigemptyset(&action.sa_mask);
					action.sa_flags = 0;
					
					// Registar o manipulador de sinais para o sinal SIGUSR1
					if (sigaction(SIGUSR1, &action, NULL) == -1) {
					perror("sigaction");
					exit(EXIT_FAILURE);
					}
					
					close(p_b[0]);
					close(1);
					dup(p_b[1]);
					
					isto = execl(ta->ficheiro, ta->ficheiro, NULL);
					exit(-1);
						
				}
				
				//pai continua a correr
				close(p_b[1]);
				ta->pidP=f;	
					while((size = read(p_b[0], res, 99))>0){
						  
							res[size] = '\0';
							if(strcmp(res, "\n")!=0){
								pthread_mutex_lock(ta->m);
									printf("\n'%s':%s", ta->ficheiro, res);
									

									for(int i=0, y=0, b=0, count=0;res[i]!='\0'; i++, y++){
										if(res[i]==' '){
											count++;
											b=y;
											y=-1;
										}
										else{
											if(count==0)
												cat[y]=res[i];
											if(count==1){
												cat[b]='\0';
												prom[y]=res[i];
											}
											if(count==2){
												prom[b]='\0';
												if(res[i]=='\n')
													dur[y]='\0';
												else
													dur[y]=res[i];
											}
										}			
									}

									for(int i=0; i<NMAXITEMS, strcmp(items[i].nome, "a")!=0; i++){
										if(strcmp(cat, items[i].categoria)==0){
											items[i].promDur=atoi(dur);
											items[i].prom=atoi(prom);
											items[i].pidProm=ta->pidP;
										}
									}
									sprintf(promocao, "Promocao nos produtos da cat.:%s de %d%%", cat, atoi(prom));
									enviaMensagemU(promocao);
								pthread_mutex_unlock(ta->m);
							}
							
						
					}
				
				close(p_b[0]);
				wait(&estado);

				for(int i=0; i<NMAXITEMS, strcmp(items[i].nome, "a")!=0; i++){
					if(ta->pidP==items[i].pidProm){
						items[i].promDur=0;
						items[i].prom=0;
						items[i].pidProm=0;
						sprintf(promocao, "O produto com ID:%d já não tem promocao", items[i].ID);
							enviaMensagemU(promocao);
					}
				}
			
	
    pthread_exit(NULL);
}

void *decreaseTime(void *leilao){
	char mensagemG[200];
    int y=0;
   	Leilao *tleilao = (Leilao*) leilao;
    do{
        for (int i = 0; i<NMAXITEMS && strcmp(tleilao[i].nome, "a")!=0; i++) {
        	if(tleilao[i].duracao<=1){
				if(strcmp(tleilao[i].nomeLic, "-")!=0){
					if(getUserBalance(tleilao[i].nomeLic)>=tleilao[i].valbase){

						updateUserBalance(tleilao[i].nomeLic, getUserBalance(tleilao[i].nomeLic)-tleilao[i].valbase);
						updateUserBalance(tleilao[i].nomeVend, getUserBalance(tleilao[i].nomeVend)+tleilao[i].valbase);
						sprintf(mensagemG, "O produto '%s' com ID=%d, da cat. '%s' foi adquirido pelo utilizador '%s' por %d€", tleilao[i].nome, tleilao[i].ID, tleilao[i].categoria, tleilao[i].nomeLic, tleilao[i].valbase);
					
					}else
						sprintf(mensagemG, "O utilizador '%s' nao teve fundos para ficar com o produto de ID=%d", tleilao[i].nomeLic, tleilao[i].ID);
				}else
					sprintf(mensagemG, "O produto '%s' com ID=%d, da cat. '%s' saiu de leilao, sem comprador", tleilao[i].nome, tleilao[i].ID, tleilao[i].categoria);
					
					saveUsersFile(fusers);
					enviaMensagemU(mensagemG);

                for (y = i; y<NMAXITEMS && strcmp(tleilao[y].nome, "a")!=0; y++) {
					if(y<NMAXITEMS-1)
                    	tleilao[y] = tleilao[y+1];
					if(y==NMAXITEMS-1){
						strcpy(tleilao[y].nome,"a");
						break;
					}
                }
				flagNMaxI=0;
            }
			if(tleilao[i].promDur<=1 && tleilao[i].prom!=0){
				tleilao[i].prom=0;
				tleilao[i].promDur=0;
				tleilao[i].pidProm=0;
			}
            tleilao[i].duracao--; 
			if(tleilao[i].prom!=0)
				tleilao[i].promDur--;
			
        }
        sleep(1);
    }while(1);

}


void handler_sigalarm(int s,siginfo_t *t, void *v){
                    unlink (BACKENDFIFO);
                    printf("\nAdeus\n");
                    exit (1);

}

int getValueProm(int i){

	if(items[i].prom!=0){
	return items[i].valcp-items[i].valcp*items[i].prom/100;
		}else
	return items[i].valcp;
	
}

/*------------------------------------------------------------------------------------------------MAIN---------------------------------------------------------------------*/
int main(char *envp[]){

	struct sigaction sa;
    sa.sa_sigaction=handler_sigalarm;
    sigaction(SIGINT, &sa, NULL);

	
	

	/*-----------variaveis de ambiente------------*/

	char *fpromotor="FPROMOTORES.txt";
	char *fitems="FITEMS.txt";
	fusers="FUSERS.txt";
	if(getenv("FPROMOTORES")){
  		fpromotor = getenv("FPROMOTORES");
	}
	if(getenv("FUSERS")){
  		fusers = getenv("FUSERS");
	}
	if(getenv("FITEMS")){
  		fitems = getenv("FITEMS");
	}
	/*--------------------------------------------*/
	/*----------------VARIAVEIS-------------------*/
	int op;

	int space=0;
	char arg1[20];
	char arg2[20];
	char arg3[20];
	char arg4[20];
	char arg5[20];
	char arg6[20];
	int b=0, c=0;
	int valido=0;

	char mensagemG[200];

	
	char filePName[30];
	sprintf(filePName,"./%s",fusers);

	int contadorClientes=0, contadorItens=0, contadorPromotores=0;
	contadorClientes = loadUsersFile(filePName);
	
	if (contadorClientes == -1){
		perror("[ERRO] - Ao abrir o arquivo de utilizadores!");
		exit (-1);
	}

	msgBF comunicacao;
	
	
	strcpy(clientesOnline[0].nome, "a");
	/*--------------------------------------------*/
	
	/*------------GUARDAR DADOS DO FICHEIRO DE ITENS NA ESTRUTURA-----------------*/
	
	int i=0, count=8;
	char cEnd;
	int ch=0;
	char palavra[20];

	
	Leilao temp[NMAXITEMS]; //para envios expecificos
	FILE *f = fopen(fitems, "r");

	if (f == NULL) {
		perror("[ERRO] - Ao abrir o arquivo de itens!");
		exit (-1);
	}
	else{
		while((cEnd = fgetc(f))!= EOF){
			ungetc (cEnd,f);
				
			while(fscanf(f, "%19s", palavra)==1){
				
				if(count==8)
					items[i].ID=atoi(palavra);
				else if(count==7)
					strcpy(items[i].nome, palavra);
				else if(count==6)
					strcpy(items[i].categoria, palavra);
				else if(count==5)
					items[i].valbase=atoi(palavra);
				else if(count==4)
					items[i].valcp=atoi(palavra);
				else if(count==3)
					items[i].duracao=atoi(palavra);
				else if(count==2)
					strcpy(items[i].nomeVend,palavra);
				else if(count==1)
					strcpy(items[i].nomeLic,palavra);
				if (count!=1)
					count--;
				else{
					items[i].prom=0;
					count=8;
					i++;
					contadorItens++;
				}		
					
				
			}		
		}
		fclose(f);
	}

	if(contadorItens==NMAXITEMS)
		flagNMaxI=1;
	
	int id=0;
	if(contadorItens==0){
		id=1;
	}else if (contadorItens>0)
		id=items[contadorItens-1].ID+1;
	else
		id=-1;

	strcpy(items[contadorItens].nome, "a");
	pthread_t tempo;

	
	
	/*----------------------------------------------------------------------------*/

	/*---------------------------------HEARTBEAT---------------------------------*/

	pthread_t checkHeartBeat;
	 if (pthread_create (&checkHeartBeat,NULL,&check_active_users,NULL)!=0)
	  	printf("heartbeat checker nao irá funcionar!");

	/*----------------------------------------------------------------------------*/

	/*---------------------------------PROMOTORES---------------------------------*/
	Promotor ativos[NMAXPROMOS];
	pthread_mutex_t  mutex;  
    pthread_mutex_init(&mutex, NULL); //inicializar

	i=0;
	count=0;
	f = fopen(fpromotor, "r");

	if (f == NULL) {
		perror("[ERRO] - Ao abrir o arquivo de promotores!");
		exit (-1);
	}
	else{
		while (fgets(palavra, sizeof(palavra), f) && count<NMAXPROMOS) {
			
			if(palavra[strlen(palavra)-1] == '\n')
				palavra[strlen(palavra) - 1] = '\0';
			strcpy(ativos[i].ficheiro,palavra);
			ativos[i].m=&mutex;
			count++;
			i++;
			

		}
		fclose(f);
	}
	contadorPromotores=count;
	printf("!%d!",contadorPromotores);
	if(contadorPromotores<NMAXPROMOS)
		strcpy(ativos[contadorPromotores].ficheiro, "a");
		
	//pthread_t tpromotores[contadorPromotores];
	
	
	for (int i=0 ; i<contadorPromotores ; i++ ){
      if (pthread_create (&ativos[i].tid,NULL,&imprimePromotor,&ativos[i])!=0)
	  	printf("promotor nao irá funcionar!");
	}

	/*if(pthread_create(&promotor,NULL,&imprimePromotor, &a)!=0)
		printf("promotor nao irá funcionar!");
	else
		printf("promotor '%s' iniciado", fpromotor);*/


	/*----------------------------------------------------------------------------*/

	if(pthread_create(&tempo,NULL,&decreaseTime, &items)!=0)
		printf("[ERRO] - IMPOSSIVEL DECRESCER O TEMPO!");

	/*---------------------------------INICIAR THREADS----------------------------*/

	/*----------------------------------------------------------------------------*/

	/*-----------------------------Interação com o cliente------------------------*/

	int fd, n, fdr;
	int sel;
	
	fd_set fds;

	if(access(BACKENDFIFO, F_OK)!=0){//verifica se o ficheiro "BACKENDFIFO" existe, SE NAO EXISTE CRIA O FIFO
    	mkfifo(BACKENDFIFO, 0600);
    	printf("Criei o fifo\n");
    }
	
	fd = open(BACKENDFIFO, O_RDWR);  //bloqueia - se não houver clientes
    	printf("Abri o fifo '%s'\n", BACKENDFIFO); //acrescentar ao nome do fifo, o pid para saber a que utilizador enviar a resposta

	

	/*----------------------------------------------------------------------------*/
	do{
		
		envia.kill=0; //se tiver a 1, da kill aos frontends

		
		printf("<COMANDO>\n>");
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

					if(strcmp(stcom, "kick")==0){

						for(k=0,j=i+1; com[j]!='\n'; j++, k++){
							ndcom[k] = com[j];
						}
						ndcom[k]='\0';
						
						int y, j, valido=0, pidK;
						for(y=0; y<NMAXUSERS && strcmp(clientesOnline[y].nome, "a")!=0; y++){
							if(strcmp(clientesOnline[y].nome,ndcom)==0){
								pidK=clientesOnline[y].pid;
								valido=1;
								break;		
							}
						}
						if(valido==1){
							for (j = y; j<NMAXUSERS && strcmp(clientesOnline[j].nome, "a")!=0; j++) {
								if(j<NMAXUSERS-1)
									clientesOnline[j] = clientesOnline[j+1];
								if(j==NMAXUSERS-1){
									strcpy(clientesOnline[j].nome,"a");
									break;
								}
								
							}	
							flagNMaxU=0;

							//enviar resposta
							envia.kill=1;
							sprintf(envia.resposta, "Foste kickado pelo administrador!");
							sprintf(fifo, FRONTENDFIFO, pidK);
							fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
							if((n= write(fdr, &envia, sizeof(respostaBF)))>0){
								close(fdr);
								printf("Enviei...%d (%d bytes)\n", pidK, n);
							
							printf("\nUtilizador '%s' foi kickado!\n", ndcom);
							}
							else
								printf("[ERRO] - ao enviar dados a utilizador");

							
						}else
							printf("[ERRO] - Utilizador '%s' não esta online!\n", ndcom);
						
               		}

					else if(strcmp(stcom, "cancel")==0){

						for(k=0,j=i+1; com[j]!='\n'; j++, k++){
							ndcom[k] = com[j];
						}
						ndcom[k]='\0';

						valido=0;
						for(int g=0; strcmp(ativos[g].ficheiro, "a")!=0 && g<NMAXPROMOS; g++){
							
							if(strcmp(ndcom, ativos[g].ficheiro)==0){
								valido=1;
								if(kill(ativos[g].pidP, SIGUSR1)!=-1){

									if (pthread_join(ativos[g].tid, NULL)!=0)
											printf("[ERRO] - pthread_join\n");
									else{
										for (int y = g; strcmp(ativos[y].ficheiro, "a")!=0 && y<NMAXPROMOS; y++) { //apagar o item da estrutura
											if(y<NMAXPROMOS-1){
												ativos[y] = ativos[y+1];
												
											}
											else if(y==NMAXPROMOS-1){
												strcpy(ativos[y].ficheiro,"a");
												ativos[y].pidP=0;
												ativos[y].tid=0;
											}
										}
										
									}

								}else{
									printf("[ERRO] - AO MATAR PROCESSO %d\n", ativos[g].pidP);
								}

								break;
							}


						}
						if(valido==1){
							printf("\nFicheiro de pormotores '%s' cancelado!\n", ndcom);
						}
						else 
							printf("\n[ERRO] - Promotor '%s' não esta ativo!\n", ndcom);
					}
				}

				else{
					

					if(strcmp(com,"users\n")==0){

						
						if(contadorClientes==-1)
							printf("Aconteceu um ERRO a abrir o ficheiro '%s'", fusers);
						
						else if(contadorClientes==0)
							printf("Não há clientes");

						else{
							printf("\nLista de utilizadores online\n");
							int g;
							for(g=0; g<NMAXUSERS && strcmp(clientesOnline[g].nome, "a")!=0;g++){

								printf("Utilizador: %s, Pass: %s, Saldo: %d\n", clientesOnline[g].nome, clientesOnline[g].pw, getUserBalance(clientesOnline[g].nome));
								
							}
							if(g==0)
								printf("Sem utilizadores online!\n");
							

						}

					}
					else if(strcmp(com,"list\n")==0){
						
						if(contadorItens==-1){
							printf("Aconteceu um ERRO a abrir o ficheiro '%s'", fitems);
						}
						else{
							printf("\nLista os itens em Leilao:\n");
							if(verificaItems(contadorItens)==1){
								
								for(int g=0; strcmp(items[g].nome, "a")!=0 && g<NMAXITEMS;g++){
									
									printf("\nID: %d, Nome: %s, Categ.: %s, VB: %d€, VC: %d€, dur: %ds, vend.: %s", items[g].ID, items[g].nome, items[g].categoria, items[g].valbase, getValueProm(g), items[g].duracao, items[g].nomeVend);
									if(strcmp(items[g].nomeLic, "-")!=0)
										printf(", ult.Lic.: %s.", items[g].nomeLic);
									else
										printf(", ainda sem licitacoes.");
									if(items[g].prom!=0)
										printf(" Em prom: %d%%, dur.:%d", items[g].prom, items[g].promDur);
								}
							printf("\n");
							}
							else
								printf("\nNão foram encontrados itens!");
						}

					}
					else if(strcmp(com,"prom\n")==0){
						int g=0;
						printf("\nLista d promotores ativos:\n");
						for(g=0; strcmp(ativos[g].ficheiro, "a")!=0 && g<NMAXPROMOS;g++){
							printf("Promotor: '%s', pid=%d\n", ativos[g].ficheiro, ativos[g].pidP);
						}
						if(g==0)
							printf("Nao ha promotores ativos\n");
					}
					else if(strcmp(com,"reprom\n")==0){

						i=0;
						count=0;
						f = fopen(fpromotor, "r");
						int valida=0;
						int guarda=0;
						Promotor temp[NMAXPROMOS];
						if (f == NULL) {
							perror("[ERRO] - Ao abrir o arquivo de promotores!");
						}
						else{

							while (fgets(palavra, sizeof(palavra), f) && count<NMAXPROMOS) {
								valida=0;
								if(palavra[strlen(palavra)-1] == '\n')
										palavra[strlen(palavra) - 1] = '\0';

								for (int g = 0; g < NMAXPROMOS && strcmp(ativos[g].ficheiro, "a")!=0; g++) {
									if (strcmp(ativos[g].ficheiro, palavra) == 0){
										temp[count++]=ativos[g];
										valida=1; // Encontrou o elemento
										guarda=g;
										break;
									}	
									
								}
								if(valida==1){
									for (int g = guarda; g < NMAXPROMOS && strcmp(ativos[g].ficheiro, "a")!=0; g++) {
										if(g<NMAXPROMOS-1){
											ativos[g] = ativos[g+1];
										}
										else if(g==NMAXPROMOS-1){
											strcpy(ativos[g].ficheiro,"a");
											ativos[g].pidP=0;
											temp[g].tid=0;
											break;
										}
									}	
									
								}else{
									strcpy(temp[count].ficheiro,palavra);
									temp[count].pidP=0;
									temp[count].tid=0;
									count++;
								}
								i++;	

							}
							fclose(f);
						}
						if(count<NMAXPROMOS)
							strcpy(temp[count].ficheiro, "a");

						for (int g = 0; g < NMAXPROMOS && strcmp(ativos[g].ficheiro, "a")!=0; g++) {
							if (kill(ativos[g].pidP, SIGUSR1)==-1)
								printf("[ERRO] - AO MATAR PROCESSO %d\n", ativos[g].pidP);
							else{
								if (pthread_join(ativos[g].tid, NULL)!=0)
									printf("[ERRO] - pthread_join\n");
							}
							
						}

						for(int g=0; g<NMAXPROMOS; g++)
							ativos[g]=temp[g];

						//pthread_t tp[count];

						for (int i=0 ; i<count ; i++ ){
							if(ativos[i].pidP==0){
								if (pthread_create (&ativos[i].tid,NULL,&imprimePromotor,&ativos[i])!=0)
									printf("promotor nao irá funcionar!");
							}
						}

					}
					else if(strcmp(com,"close\n")==0){

						f = fopen(fitems, "w");

						if (f == NULL) {
							perror("Erro ao abrir o arquivo");
						}
						for(int g=0; strcmp(items[g].nome, "a")!=0 && g<NMAXITEMS;g++)
							fprintf(f, "%d %s %s %d %d %d %s %s\n", items[g].ID, items[g].nome, items[g].categoria, items[g].valbase, items[g].valcp, items[g].duracao, items[g].nomeVend, items[g].nomeLic);
						fclose(f);

						printf("\nA plataforma vai encerrar\n");

						strcpy(mensagemG, "A plataforma vai encerrar");
						enviaMensagemU(mensagemG);

						close(fd);
						unlink(BACKENDFIFO);
							
						
						//pthread_join(promotor, NULL);
						exit(-1);
					}
					else
						printf("\nComando invalido!\n");
				}

			


			
			
		}
		else if(sel>0 && FD_ISSET(fd, &fds)){

			
				if((n= read(fd, &comunicacao, sizeof(msgBF)))>0){ // HA DADOS.... NAO BLOQUEIA
					printf("Recebi...%d %s %d (%d bytes)\n", comunicacao.codMsg, comunicacao.mensagem, comunicacao.pid, n);

					if(comunicacao.codMsg==0){//iniciar sessão

						space=0;
						b=0; 
						c=0;
						for(int i=0; comunicacao.mensagem[i] != '\0'; i++, c++){
							if(comunicacao.mensagem[i]==' '){
								space+=1;
								b=c;
								c=-1;
							}
							else{
								if(space==0)
									arg1[c]=comunicacao.mensagem[i];
								if(space==1){
									arg1[b]='\0';
									arg2[c]=comunicacao.mensagem[i];
								}
							}		
						}arg2[c]='\0';

						int estaOnline=0;

						for(int i=0; i<NMAXUSERS; i++){
							if(strcmp(clientesOnline[i].nome,arg1)==0 && strcmp(clientesOnline[i].pw,arg2)==0){
								estaOnline=1;
								break;
							}
						}
						if(estaOnline==0 && flagNMaxU==0){
						//verifica na estrutura se existe
						
							if(isUserValid(arg1,arg2)==1){
									strcpy(comunicacao.arg3, "1");
									printf("%s",comunicacao.arg3);

									//meter utilizador na estrutura online
									for(int i=0;i<NMAXUSERS;i++){
										if(strcmp(clientesOnline[i].nome, "a")==0){

											strcpy(clientesOnline[i].nome,arg1);
											strcpy(clientesOnline[i].pw,arg2);
											clientesOnline[i].pid=comunicacao.pid;
											clientesOnline[i].last_heartbeat=time(NULL);;
											if(i<NMAXUSERS-1){
												strcpy(clientesOnline[i+1].nome, "a");
											}
											else if (i==NMAXUSERS-1)
												flagNMaxU=1;
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
								printf("Enviei...%d %d %s (%d bytes)\n", comunicacao.codMsg, comunicacao.pid, comunicacao.arg3, n);
							}
							else
								printf("[ERRO] - ao enviar dados a utilizador");
						}else{
							if(estaOnline==1)
								sprintf(comunicacao.resposta, "Utilizador já se encntra online!");
							else if(flagNMaxU==1)
								sprintf(comunicacao.resposta, "O número máximo de utilizadores foi atingido!");
							//enviar resposta
							sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
							fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
							if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
								close(fdr);
								printf("Enviei...%d %d %s (%d bytes)\n", comunicacao.codMsg, comunicacao.pid, comunicacao.arg3, n);
							}
							else
								printf("[ERRO] - ao enviar dados a utilizador");
						}

					}

					else if(comunicacao.codMsg==1){
						
						valido=0;
						if(flagNMaxI!=1){
							
							space=0;
							b=0; 
							c=0;
							for(int i=0; comunicacao.mensagem[i] != '\0'; i++, c++){
								if(comunicacao.mensagem[i]==' '){
									space++;
									b=c;
									c=-1;
								}
								else{
									if(space==0)
										arg1[c]=comunicacao.mensagem[i];
									if(space==1){
										arg1[b]='\0';
										arg2[c]=comunicacao.mensagem[i];}
									if(space==2){
										arg2[b]='\0';
										arg3[c]=comunicacao.mensagem[i];}
									if(space==3){
										arg3[b]='\0';
										arg4[c]=comunicacao.mensagem[i];}
									if(space==4){
										arg4[b]='\0';
										arg5[c]=comunicacao.mensagem[i];}
									if(space==5){
										arg5[b]='\0';
										arg6[c]=comunicacao.mensagem[i];}
								}
							}arg6[c]='\0';
							
							if(atoi(arg3)>0 && atoi(arg4)>0 && atoi(arg5)>0){

								for(int i=0; i<NMAXITEMS; i++){
									if(strcmp(items[i].nome, "a")==0){
										items[i].ID=id++;
										strcpy(items[i].nome,arg1);
										strcpy(items[i].categoria,arg2);
										items[i].valbase=atoi(arg3);
										items[i].valcp=atoi(arg4);
										items[i].duracao=atoi(arg5);
										strcpy(items[i].nomeVend, arg6);
										strcpy(items[i].nomeLic, "-");
										items[i].prom=0;
										if(i<NMAXITEMS-1)
											strcpy(items[i+1].nome,"a");
										else if(i==NMAXITEMS-1){
											flagNMaxI=1;
										}
										break;
									}
								}
							strcpy(comunicacao.resposta, "ITEM ADICIONADO AO LEILÃO!");
							sprintf(mensagemG, "Produto '%s' com ID=%d, da cat. '%s' adicionado ao leilao pelo utilizador '%s' com VB:%d€ e VC:%d€", arg1, id-1, arg2, arg6, atoi(arg3), atoi(arg4));
							valido=1;
							}else
								strcpy(comunicacao.resposta, "[ERRO] - DADOS INVÁLIDOS!");
						}else
							strcpy(comunicacao.resposta, "[ERRO] - NÚMERO MÁXIMO DE ITENS À VENDA!");

						sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
						fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
						if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
							close(fdr);
							printf("Enviei...%d, %s (%d bytes)\n", comunicacao.pid, comunicacao.resposta, n);
						}
						else
							printf("[ERRO] - ao enviar dados a utilizador");

						if(valido==1)
							enviaMensagemU(mensagemG);
						
					}
					else if(comunicacao.codMsg==2){

						//enviar resposta
						sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
						fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
						if((n= write(fdr, &items, sizeof(items)))>0){
							close(fdr);
							printf("Enviei... (%d bytes)\n", n);
						}
						else
							printf("[ERRO] - ao enviar dados a utilizador");

					}
					else if(comunicacao.codMsg==3){

						int y=0;
						for (int i = 0; i<NMAXITEMS && strcmp(items[i].nome, "a")!=0; i++) {
							if(strcmp(items[i].categoria, comunicacao.mensagem)==0){
								temp[y]=items[i];
								y++;
							}
						}

						strcpy(temp[y].nome, "a");
						//enviar resposta
						sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
						fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
						if((n= write(fdr, &temp, sizeof(temp)))>0){
							close(fdr);
							printf("Enviei... (%d bytes)\n", n);
						}
						else
							printf("[ERRO] - ao enviar dados a utilizador");
						
						strcpy(temp[0].nome, "a");
						
					}
					else if(comunicacao.codMsg==4){
						
						int y=0;
						for (int i = 0; i<NMAXITEMS && strcmp(items[i].nome, "a")!=0; i++) {
							if(strcmp(items[i].nomeVend, comunicacao.mensagem)==0){
								temp[y]=items[i];
								y++;
							}
						}
						strcpy(temp[y].nome, "a");
						//enviar resposta
						sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
						fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
						if((n= write(fdr, &temp, sizeof(temp)))>0){
							close(fdr);
							printf("Enviei... (%d bytes)\n", n);
						}
						else
							printf("[ERRO] - ao enviar dados a utilizador");
						
						strcpy(temp[0].nome, "a");


					}
					else if(comunicacao.codMsg==5){

						int y=0;
						for (int i = 0; i<NMAXITEMS && strcmp(items[i].nome, "a")!=0; i++) {
							if(items[i].valbase < atoi(comunicacao.mensagem)){
								temp[y]=items[i];
								y++;
							}
						}

						strcpy(temp[y].nome, "a");
						//enviar resposta
						sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
						fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
						if((n= write(fdr, &temp, sizeof(temp)))>0){
							close(fdr);
							printf("Enviei... (%d bytes)\n", n);
						}
						else
							printf("[ERRO] - ao enviar dados a utilizador");
						
						strcpy(temp[0].nome, "a");

					}
					else if(comunicacao.codMsg==6){

						int y=0;
						for (int i = 0; i<NMAXITEMS && strcmp(items[i].nome, "a")!=0; i++) {
							if(items[i].duracao < atoi(comunicacao.mensagem)){
								temp[y]=items[i];
								y++;
							}
						}

						strcpy(temp[y].nome, "a");
						//enviar resposta
						sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
						fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
						if((n= write(fdr, &temp, sizeof(temp)))>0){
							close(fdr);
							printf("Enviei... (%d bytes)\n", n);
						}
						else
							printf("[ERRO] - ao enviar dados a utilizador");
						
						strcpy(temp[0].nome, "a");

						

					}
					else if(comunicacao.codMsg==7){

						time_t t = time(NULL);
						struct tm tm = *localtime(&t);
						
						sprintf(comunicacao.resposta, "%d-%d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);	//data

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

						
						b=0; 
						c=0;
						space=0;
						valido=0;
						for(int i=0; comunicacao.mensagem[i] != '\0'; i++, c++){
							if(comunicacao.mensagem[i]==' '){
								space++;
								b=c;
								c=-1;
							}
							else{
								if(space==0){
									arg1[c]=comunicacao.mensagem[i];
								}
								if(space==1){
									arg1[b]='\0';
									arg2[c]=comunicacao.mensagem[i];
								}
								if(space==2){
									arg2[b]='\0';
									arg3[c]=comunicacao.mensagem[i];
								}
							}
						}arg3[c]='\0';
						
						for (int i = 0 ; i<NMAXITEMS && strcmp(items[i].nome, "a")!=0; i++) {
							if(items[i].ID == atoi(arg1)){


								if(strcmp(items[i].nomeVend, arg3)!=0){
									if(getValueProm(i)==atoi(arg2) && getUserBalance(arg3)>=atoi(arg2)){ //se fizer o pedido compra já
										
										updateUserBalance(arg3, getUserBalance(arg3)-getValueProm(i));
										updateUserBalance(items[i].nomeVend, getUserBalance(items[i].nomeVend)+getValueProm(i));
										sprintf(mensagemG, "O produto '%s' com ID=%d, da cat. '%s' foi comprado pelo utilizador '%s' por %d€", items[i].nome, items[i].ID, items[i].categoria, arg3, atoi(arg2));

										for (int y = i; y<NMAXITEMS && strcmp(items[y].nome, "a")!=0; y++) { //apagar o item da estrutura
											if(y<NMAXITEMS-1)
												items[y] = items[y+1];
											if(y==NMAXITEMS-1){
												strcpy(items[y].nome,"a");
												flagNMaxI=0;
												break;
											}
										}
			
										strcpy(comunicacao.resposta, "Comprado pelo VC com sucesso!");
										valido=1;
									}
									else if(items[i].valbase<atoi(arg2) && getUserBalance(arg3)>=atoi(arg2)){ //licitacao do produto
										strcpy(items[i].nomeLic,arg3);
										items[i].valbase=atoi(arg2);

										strcpy(comunicacao.resposta, "Produto Licitado com sucesso!");
										sprintf(mensagemG, "O produto com ID=%d foi Licitado pelo utilizador '%s' no valor de %d€", atoi(arg1), arg3, atoi(arg2));
										valido=1;
									}else
										strcpy(comunicacao.resposta, "Nao tem fundos necessários!");
								}else
									strcpy(comunicacao.resposta, "Não pode comprar o próprio produto!");
								break;
							}else
								strcpy(comunicacao.resposta, "O produto mencionado nao existe!");
						}
						if(valido!=1){
							strcpy(comunicacao.resposta, "O produto mencionado nao existe!");
						}
						saveUsersFile(fusers);

						//enviar resposta
							sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
							fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
							if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
								close(fdr);
								printf("Enviei..%d (%d bytes)\n", comunicacao.pid, n);
							}else
								printf("[ERRO] - ao enviar dados a utilizador");

						if(valido==1){

							enviaMensagemU(mensagemG);
						}

					}
					else if(comunicacao.codMsg==9){


						if(getUserBalance(comunicacao.mensagem)!=-1)
							sprintf(comunicacao.resposta, "O seu saldo é %d€", getUserBalance(comunicacao.mensagem));
						else
							strcpy(comunicacao.resposta, "[ERRO] - AO PROCURAR PELO UTILIZADOR");
							
						
						//enviar resposta
							sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
							fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
							if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
								close(fdr);
								printf("Enviei..%d (%d bytes)\n", comunicacao.pid, n);
							}else
								printf("[ERRO] - ao enviar dados a utilizador");

					}
					else if(comunicacao.codMsg==10){

							
							int verUpdate=0;
							
							for(int g=0;g<NMAXUSERS && strcmp(clientesOnline[g].nome, "a")!=0;g++){
								if(clientesOnline[g].pid==comunicacao.pid){		
									updateUserBalance(clientesOnline[g].nome, getUserBalance(clientesOnline[g].nome)+atoi(comunicacao.mensagem));
									verUpdate=1;
									
									break;
								}
							}
							saveUsersFile(fusers);

							if(verUpdate==1){
								sprintf(comunicacao.resposta, "Saldo atualizado com sucesso!");
							}else
								sprintf(comunicacao.resposta, "[ERRO] - Ao atualizar!");

							//enviar resposta
							sprintf(fifo, FRONTENDFIFO, comunicacao.pid);
							fdr = open(fifo, O_WRONLY);  //bloqueia - se não houver clientes
							if((n= write(fdr, &comunicacao, sizeof(msgBF)))>0){
								close(fdr);
								printf("Enviei.. %d(%d bytes)\n", comunicacao.pid, n);
							}else
								printf("[ERRO] - ao enviar dados a utilizador");


					}
					else if(comunicacao.codMsg==11){

						int y, j, valido=0;
						for(y=0; y<NMAXUSERS && strcmp(clientesOnline[y].nome, "a")!=0; y++){
							if(clientesOnline[y].pid==comunicacao.pid){
								valido=1;
								break;		
							}
						}
						if(valido==1){
							for (j = y; j<NMAXUSERS && strcmp(clientesOnline[j].nome, "a")!=0; j++) {
								if(j<NMAXUSERS-1)
									clientesOnline[j] = clientesOnline[j+1];
								if(j==NMAXUSERS-1){
									strcpy(clientesOnline[j].nome,"a");
									break;
								}
								
							}	
							flagNMaxU=0;
						}
						else
							printf("[ERRO] - ao desconectar utilizador %s", clientesOnline[y].nome);
						
					}else if(comunicacao.codMsg==12){
						for (int j = 0; j<NMAXUSERS && strcmp(clientesOnline[j].nome, "a")!=0; j++) {
							if(comunicacao.pid==clientesOnline[j].pid)
								clientesOnline[j].last_heartbeat=time(NULL);
						}
					}

				}else
					printf("[ERRO] - ao receber dados do utiliziador");
				
			
		}

	}while(1);

	
}
