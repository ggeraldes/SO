//===================== BACKEND =====================//
#include "header.h"

/*--------Variaveis Globais-------*/
int flagNMaxI=0;
char fifo[40];
//necessário pela impossibilidade mais do que uma struct para thread
Cliente clientesOnline[NMAXUSERS]; 
respostaBF envia;
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
				int isto=0;
				pipe(p_b);
				char  res[100];

				int f = fork();
				if(f == 0){
					//filho corre o promotor
					pid=getpid();
					close(p_b[0]);
					close(1);
					dup(p_b[1]);

					isto = execl(ta->nomePromotor, ta->nomePromotor, NULL);
					exit(-1);
						
				}
					
				//pai continua a correr
				close(p_b[1]);
				
				while((size = read(p_b[0], res, 99))>0 && ta->kill!=1){
					res[size] = '\0';
					if(strcmp(res,"\n")!=0)
						printf("\nPromotor: %s", res);

				}
				close(p_b[0]);
				

				wait(&estado);

			
	
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
            tleilao[i].duracao--; 
        }
        sleep(1);
    }while(1);

}


void handler_sigalarm(int s,siginfo_t *t, void *v){
                    unlink (BACKENDFIFO);
					
                    printf("\nAdeus\n");
                    exit (1);

}


int main(char *envp[]){

	struct sigaction sa;
    sa.sa_sigaction=handler_sigalarm;
    sigaction(SIGINT, &sa, NULL);

	/*-----------variaveis de ambiente------------*/

	char *fpromotor="promotor_oficial";
	char *fusers="FUSERS.txt";
	char *fitems="FITEMS.txt";

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

	/*------------thread promotores---------------*/
	killThread a;
	pthread_t promotor;
	a.kill=0;
	strcpy(a.nomePromotor,fpromotor);

	if(pthread_create(&promotor,NULL,&imprimePromotor, &a)!=0)
		printf("promotor nao irá funcionar!");
	else
		printf("promotor '%s' iniciado", fpromotor);


	/*--------------------------------------------*/
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
	
	respostaBF envia;
	envia.kill=0;

	
	char filePName[30];
	sprintf(filePName,"./%s",fusers);

	int contadorClientes=0, contadorItens=0;
	contadorClientes = loadUsersFile(filePName);
	

	msgBF comunicacao;
	
	int flagNMaxU=0;
	strcpy(clientesOnline[0].nome, "a");
	
	/*------------GUARDAR DADOS DO FICHEIRO DE ITENS NA ESTRUTURA-----------------*/
	
	int i=0, count=8;
	char cEnd;
	int ch=0;
	char palavra[20];

	Leilao items[NMAXITEMS];
	Leilao temp[NMAXITEMS]; //para envios expecificos
	FILE *f = fopen(fitems, "r");

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
				count=8;
				i++;
				contadorItens++;
			}		
				
			
		}		
	}
	fclose(f);

	if(contadorItens==NMAXITEMS)
		flagNMaxI=1;
	
	int id=items[contadorItens-1].ID+1;
	strcpy(items[contadorItens].nome, "a");
	pthread_t tempo;

	if(pthread_create(&tempo,NULL,&decreaseTime, &items)!=0)
		printf("[ERRO] - IMPOSSIVEL DECRESCER O TEMPO!");
	
	/*----------------------------------------------------------------------------*/

	/*-----------------------------Interação com o cliente------------------------*/

	int fd, n, fdr, fd1;
	int sel, sel1;
	
	fd_set fds;

	if(access(BACKENDFIFO, F_OK)!=0){//verifica se o ficheiro "BACKENDFIFO" existe, SE NAO EXISTE CRIA O FIFO
    	mkfifo(BACKENDFIFO, 0600);
    	printf("Criei o fifo\n");
    }
	
	fd = open(BACKENDFIFO, O_RDWR);  //bloqueia - se não houver clientes
    	printf("Abri o fifo '%s'\n", BACKENDFIFO); //acrescentar ao nome do fifo, o pid para saber a que utilizador enviar a resposta

	

	/*----------------------------------------------------------------------------*/
	do{


		
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
						
               		}

					else if(strcmp(stcom, "cancel")==0){

						for(k=0,j=i+1; com[j]!='\n'; j++, k++){
							ndcom[k] = com[j];
						}
						ndcom[k+1]='\0';
					
						if(strcmp(ndcom, fpromotor)==0){
							a.kill=1;
							printf("\nFicheiro de pormotores cancelado!\n");
						}
						else 
						printf("\nComando invalido!\n");
					}
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
							for(g=0; g<NMAXUSERS && strcmp(clientesOnline[g].nome, "a")!=0;g++){

								printf("Utilizador: %s, Pass: %s, Saldo: %d\n", clientesOnline[g].nome, clientesOnline[g].pw, getUserBalance(clientesOnline[g].nome));
								
							}
							if(g==0)
								printf("Sem utilizadores online!\n");
							

						}

					}
					else if(strcmp(com,"list\n")==0){

						printf("\nLista os itens a venda\n");
						if(verificaItems(contadorItens)==1){
							for(int g=0; strcmp(items[g].nome, "a")!=0 && g<NMAXITEMS;g++){
								printf("\nID: %d, Nome: %s, Categ.: %s, VB: %d€, VC: %d€, dur: %ds, vend.: %s", items[g].ID, items[g].nome, items[g].categoria, items[g].valbase, items[g].valcp, items[g].duracao, items[g].nomeVend);
								if(strcmp(items[g].nomeLic, "-")!=0)
									printf(", ult.Lic.: %s", items[g].nomeLic);
								else
									printf(", ainda sem licitacoes.");
							}
						printf("\n");
						}
						else
							printf("\nNão foram encontrados itens!");

					}
					else if(strcmp(com,"prom")==0){
						printf("\nLista os promotores\n");
					}
					else if(strcmp(com,"reprom")==0){
						printf("\nAtualiza os promotores\n");
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
							
						a.kill=1;	
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
						
						for (int i = 0; i<NMAXITEMS && strcmp(items[i].nome, "a")!=0; i++) {
							if(items[i].ID == atoi(arg1)){
								if(strcmp(items[i].nomeVend, arg3)!=0){
									if(items[i].valcp==atoi(arg2) && getUserBalance(arg3)>=atoi(arg2)){ //se fizer o pedido compra já
										
										updateUserBalance(arg3, getUserBalance(arg3)-items[i].valcp);
										updateUserBalance(items[i].nomeVend, getUserBalance(items[i].nomeVend)+items[i].valcp);
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
							printf("!%d!",i);
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
						
					}

				}else
					printf("[ERRO] - ao receber dados do utiliziador");
				
			
		}

	}while(1);

	
}
