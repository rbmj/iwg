#define _GNU_SOURCE
#include <dlfcn.h>
#include<stdio.h>
 
typedef int (*orig_open_f_type)(const char *pathname, int flags);
 
int open(const char *pathname, int flags, ...)
{
    /* Some evil injected code goes here. */
    printf("A loser pwned n00b used open(...) to access '%s'!!!\n", pathname); 
 
    orig_open_f_type orig_open;
    
    // vv this is why #define and dlfcn are needed
    orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open"); 

    return orig_open(pathname,flags);
}
