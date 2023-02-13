#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main(){
int a=10, res;

printf("inicio...");
res=fork();
if(res == 0)
  a++; // filho
else
  a--; // pai
printf("\na=%d\n", a);
printf("fim\n");

return 0;
}
