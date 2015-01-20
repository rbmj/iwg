#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void foo(char *s){
    char t[200];
    strcpy(t,s);
    printf(s);
}

void hello(){
    printf("Hello, fucka\n");
}

int main(int argc, char *argv[]){
    hello();
    foo(argv[1]);
}
