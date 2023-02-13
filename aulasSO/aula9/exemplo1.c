
#include <sys/select.h>

int max (int a, int b){
return (a>b) ? a:b;
}

int main(){
    int nfd;
    fd_set read_fds;
    struct timeval tv;
    char msg[30];
    char xpto[30];

    int fdRecebe1= open (ESCUTA_a,O_RDWR);
    int fdRecebe2= open (ESCUTA_b,O_RDWR);

do{

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    FD_ZERO (&read_fds);
    FD_SET (0, &read_fds);
    FD_SET (fdRecebe1, &read_fds);
    FD_SET (fdRecebe2, &read_fds);
    int maior= max(fdRecebe1, fdRecebe2)+1;

    nfd = select(maior,&read_fds, NULL, NULL, &tv);

    if(__FD_ISSET(0,&read_fds)){
        fgets(xpto, sizeof(xpto),stdin);
        if(strcmp(xpto,"terminar\n")==0)
        exit();
    }

    if(FD_ISSET(fdRecebe1, &read_fds)){
        int size=read(fdRecebe1, &msg, sizeof(msg));
        if(size==sizeof(msg)){
            //resposta

        }


    }
    if(FD_ISSET(fdRecebe2, &read_fds)){

        int size= read(fdRecebe2, &msg, sizeof(msg));
        if(size==sizeof(msg)){
            //resposta

        }


    }




}while(1);




}