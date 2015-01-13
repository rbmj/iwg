#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

void bad(){
  printf("BAD\n");
}

void vuln(char * s){

  char buf[BUF_SIZE];
  
  static int test_val = 0x00414141; //"AAA\0" as an int


  strncpy(buf, s, BUF_SIZE);

  printf("Right: ");
  printf("%s", buf);
  printf("\n\n");

  printf("Wrong: ");
  printf(buf);      //<------!!!!!
  printf("\n\n");

  printf("[*] test_val @ 0x%0x = %d 0x%08x\n", &test_val,test_val,test_val);
}

int main(int argc, char * argv[]){

  if(argc >=2){
    vuln(argv[1]);
  }

  exit(0);

}
