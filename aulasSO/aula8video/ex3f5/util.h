#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_SRV "canal"
#define FIFO_CLI "cli"

typedef struct {

    int num1, num2;
    char op;
    float res;

}PEDIDO;