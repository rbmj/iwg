// from http://rafalcieslak.wordpress.com/2013/04/02/dynamic-linker-tricks-using-ld_preload-to-cheat-inject-features-and-investigate-programs/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
int main(){
  srand(time(NULL));
  int i = 10;
  while(i--) printf("%d\n",rand()%100);
  return 0;
}

