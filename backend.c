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

void kill_process_child(int f){
	 kill(f,SIGKILL);
}

int main(){

	int i=0, count=0;
	char c;
	char palavra[20];
	int contadorClientes=0, contadorItens=0;
	contadorClientes = loadUsersFile("./FUSERS.txt");
	Cliente clientes[contadorClientes];
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

	contadorItens+=1;

	Item itens[contadorItens];
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
	/*----------------------------------------------------------------------------*/

	do{
		int op;
		printf("\n1 - Executar promotor\n2 - Comandos\n3 - Leitura do ficheiro de itens\n4 - Gestão Utilizadores\n\nDeseja testar que funcionalidade: ");
		scanf("%d", &op);
		printf("\n");

		if(op==1){

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
    	

		}
		else if(op==2){

			char com[50];
			char stcom[50];
			char ndcom[50];

			printf("\nComando: ");
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
				}
				else if(strcmp(com,"list")==0){
					printf("\nLista os itens a venda\n");
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



		else if(op==3){

			for(int g=0; g<contadorItens;g++)
					printf("\nID: %d, Nome: %s, Categ.: %s, VB: %d, VC: %d", itens[g].ID, itens[g].nome, itens[g].categoria, itens[g].valbase, itens[g].valcp);

			printf("\n");
		}

		else if(op==4){

		
			if(contadorClientes==-1)
				printf("Aconteceu um ERRO a abrir o ficheiro 'FUSERS.txt'");
			
			else if(contadorClientes==0)
				printf("Não há clientes");

			else{
				int res=0;
				for(int g=0; g<contadorClientes;g++){
					printf("\nUtilizador: %s, Pass: %s, Saldo: %d", clientes[g].nome, clientes[g].pw, clientes[g].saldo);

					res = updateUserBalance(clientes[g].nome, clientes[g].saldo-1);
					
					if(res==-1)
					printf("\n[ERROR]-updateUserBalance\n");
					else if(res==0)
					printf("\nUtilizador não encontrado\n");
					else
					clientes[g].saldo-=1;
				}

				res=saveUsersFile("FUSERS.txt");
				if(res==0)
				printf("\nUtilizador guardados com sucesso\n");
				else 
				printf("\n[ERROR]-saveUsersFile\n");

			}
			
				
		}

		else if(op==5){


		}
		else
		printf("Introduza uma opcao valida(1 ou 2)");
	}while(1);

}
