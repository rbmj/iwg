#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_ARGS 0 
#define ERROR_VALUE 1

#define BUF_SIZE 512

void usage(char * name){
  printf("USAGE: "
"%s num string\n"
"\n"
"print string num times\n",
name);
}

void debug(int errornum, char *msg){

  char error_buf[BUF_SIZE];

  switch(errornum){
  case ERROR_ARGS:
    snprintf(error_buf, BUF_SIZE, "ERROR: Invalid Args: %s \n", msg);
    break;
  case ERROR_VALUE:
    snprintf(error_buf, BUF_SIZE, "ERROR: Invalid Value: %s \n", msg);
    break;
  }
    
  fprintf(stderr, error_buf); //vulnerable printf!

}



int main(int argc, char *argv[]){

  int i;

  if (argc < 3){
    debug(ERROR_ARGS, "Not enough arguments");
    usage(argv[0]);
    return 1;
  }

  if( (i = atoi(argv[1])) <= 0){
    debug(ERROR_VALUE, argv[1]);
    usage(argv[0]);
    return 1;
  }
      

  while(i--){
    printf("%s\n", argv[2]); //safe printf!
  }
  
  return 0;
}
