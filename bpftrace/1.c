#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int fun1(int a) {
   return a+10;
}
int fun2(int a, int b) {
   return a*b;
}
int fun3(int a, int b){
   return a+b;
}
int main(int argc,char * argv[])
{
   int a = 2,b=4;
   printf("a*b=%d\n",fun2(a,b));
   getchar();
   printf("a+b=%d\n",fun3(a,b));
   getchar();
   printf("a+10=%d\n",fun1(a));
}
