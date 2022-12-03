//===================== BACKEND =====================//



#include "header.h"





int main(int argc, char*argv[]){



	do{



		int op;

		printf("\n1 - Executar promotor\n2 - Comandos\n3 - Leitura do ficheiro de itens\n4 - Gestão Utilizadores\n\nDeseja testar que funcionalidade: ");



		scanf("%d", &op);
		printf("\n");


	if(op==1){

/*	char str[40];
	int continua=1,pid, res, estado;
	int canal[2], n;

	pid=getpid();

	
	    pipe(canal);
	    res=fork();
	    if(res == 0){  // FILHO
	      pid = getpid();

	      close(canal[0]);// extremidade de leitura
	      close(1); 
	      dup(canal[1]);//coloca a extremidade de escrita no descritor 1

		  execl("./promotor_oficial", "promotor_oficial", NULL);
              printf("[%5d]Não consegui executar a aplicação\n", pid);
	      exit(7);
	    }
	  printf("[%d] PAI:Criei o filho PID=%d..\n", pid,res);
	close(canal[1]);//extremidade de escrita
	while((n=read(canal[0], str, 39))>0){
	str[n]='\0';
		 printf("%s", str);//o pai escreve

	}
	
	close(canal[0]); //extremidade de leitura
	  
	  */
	

int p_b[2];
int size;
pipe(p_b);
char  res[100];
int f = fork();
if (f == 0) {
    //filho corre o promotor
    close(p_b[0]);
    close(1);
    dup(p_b[1]);

    execlp("./promotor_oficial", "promotor_oficial", NULL);
    exit(-1);
}
    //pai continua a correr
    close(p_b[1]);
	while((size = read(p_b[0], res, 99))>0){
    res[size] = '\0';

    printf("Promotor: %s\n", res);
	}
    close(p_b[0]);


/*	int fd_in[2], fd_out[2];
   		pipe(fd_in);

    		pipe(fd_out);

    		int to_class = fd_in[1];

    		int from_class = fd_out[0];

    		if(fork() == 0) {

        	// filho

        		close(0);

        		dup(fd_in[0]);

        		close(fd_in[0]);

        		close(fd_in[1]);



        		close(1);

        		dup(fd_out[1]);

        		close(fd_out[0]);

        		close(fd_out[1]);



        		execl("./promotor_oficial", "./promotor_oficial", NULL);

    		}*/
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

		int count=5, saldo, res;
	char c;
	char *nome;
	int id, precoB, precoCP;
	FILE *f = fopen("FITEMS.txt", "r");
	while((c = fgetc(f))!= EOF){
		ungetc (c,f);
    	if(f){
			
          char palavra[20];
		  
            while(fscanf(f, "%19s", palavra)==1){
				
            if(count==5){
						id=atoi(palavra);
                        printf("id: %d, ", id);
						
                count--;
            }
            else if(count==4){
						printf("Nome: %s, ", palavra);
                count--;
            }
            else if(count==3){
                        printf("Categoria: %s, ", palavra);
				count--;
            }
			else if(count==2){
						precoB=atoi(palavra);
                        printf("Preço Base: %d, ", precoB);
				count--;
            }
			else if(count==1){
					precoCP=atoi(palavra);
                        printf("Preço Compra já: %d\n", precoCP);
                count=5;
				
            }
		
        }
		

        fclose(f);
		
    	}
	}


	}



	else if(op==4){


	
	if(loadUsersFile("./FUSERS.txt")==-1){

	printf("Aconteceu um ERRO");
	exit(1);
	}
	


	int count=3, saldo, res;
	char c;
	char *nome;
	FILE *f = fopen("FUSERS.txt", "r");
	while((c = fgetc(f))!= EOF){
		ungetc (c,f);
    	if(f){
			
          char palavra[20];
		  
            while(fscanf(f, "%19s", palavra)==1){
				
            if(count==3){
                        printf("Utilizador: %s, ", palavra);
						nome=palavra;
						saldo = getUserBalance(nome);
						res=updateUserBalance(nome, saldo-1);
						if(res==-1)
						printf("\n[ERROR]-updateUserBalance\n");
						else if(res==0)
						printf("\nUtilizador não encontrado\n");
                count--;
            }
            else if(count==2){
                count--;
            }
			
		
            else if(count==1){
                        printf("Saldo: %s\n", palavra);
                count=3;
				
            }
		
        }
		res=saveUsersFile("FUSERS.txt");
		if(res==0)
		printf("\nUtilizador guardados com sucesso\n");
		else 
		printf("\n[ERROR]-saveUsersFile\n");

        fclose(f);
		
    	}
	}



}

else if(op==5){













}
	else

		printf("Introduza uma opcao valida(1 ou 2)");



	}while(1);

}
