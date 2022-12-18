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

void handler_sigalarm(int s,siginfo_t *t, void *v){
                    unlink (BACKENDFIFO);
                    printf("\nadeus\n");
                    exit (1);

}

int main(){

	struct sigaction sa;
    sa.sa_sigaction=handler_sigalarm;
    sigaction(SIGINT, &sa, NULL);

	int op;

	int i=0, count=0;
	char c;
	char palavra[20];
	int contadorClientes=0, contadorItens=0;
	contadorClientes = loadUsersFile("./FUSERS.txt");
	Cliente clientes[contadorClientes];

	Cliente clientesOnline[contadorClientes];
	strcpy(clientesOnline[0].nome, "a");

	/* GUARDAR DADOS DO FICHEIRO DE UTILIZADORES NA ESTRUTURA*/
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

	verificaExistencia utilizador;
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


			scanf("%d", &op);
			
			
			
			

			/*-----------------PROMOTOR-------------------
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
				size = read(p_b[0], res, 99);
				res[size] = '\0';
				printf("Promotor: %s\n", res);
				close(p_b[0]);

				wait(&estado);

			*/

				char com[50];
				char stcom[50];
				char ndcom[50];

				scanf("%s", com);


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
					}
				}

				else{
					if(strcmp(com,"users")==0){

						printf("\nLista os utilizadores\n");
						if(contadorClientes==-1)
							printf("Aconteceu um ERRO a abrir o ficheiro 'FUSERS.txt'");
						
						else if(contadorClientes==0)
							printf("Não há clientes");

						else{
							//int res=0;
							int g;
							for(g=0; g<contadorClientes;g++){

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
					else if(strcmp(com,"list")==0){

						printf("\nLista os itens a venda\n");
						if(verificaItems(contadorItens)==1){
							for(int g=0; g<contadorItens;g++)
								printf("\nID: %d, Nome: %s, Categ.: %s, VB: %d, VC: %d", itens[g].ID, itens[g].nome, itens[g].categoria, itens[g].valbase, itens[g].valcp);

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
					else if(strcmp(com,"close")==0){
						printf("\nA plataforma vai encerrar\n");
						exit(-1);
					}
					else
						printf("\nComando invalido!\n");
				}

			


			
			
		}
		else if(sel>0 && FD_ISSET(fd, &fds)){
			n= read(fd, &utilizador, sizeof(verificaExistencia)); // HA DADOS.... NAO BLOQUEIA
            printf("\nRecebi...%s, %s, %d, %d (%d bytes)\n", utilizador.nome, utilizador.pwd, utilizador.pid, utilizador.validacao, n);

			//verifica na estrutura se existe
			for(int j=0; j<contadorClientes; j++){

				if(strcmp(clientes[j].nome, utilizador.nome)==0 && strcmp(clientes[j].pw, utilizador.pwd)==0){
					utilizador.validacao=1;
					
					//meter utilizador na estrutura online
					for(int i=0;i<contadorClientes;i++){
						if(strcmp(clientesOnline[i].nome, "a")==0){

							strcpy(clientesOnline[i].nome, clientes[j].nome);
							strcpy(clientesOnline[i].pw, clientes[j].pw);
							clientesOnline[i].saldo=clientes[j].saldo;

							strcpy(clientesOnline[i+1].nome, "a");
							break;
						}
					}
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

	}while(1);


}
