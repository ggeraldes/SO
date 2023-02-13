#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#define FIFO_SRV "canal"
#define FIFO_SRV1 "canal1"
#define FIFO_CLI "cli%d"

typedef struct {

    int num1, num2;
    char op;
    float res;
    int pid; //remetente 

}PEDIDO;