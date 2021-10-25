#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
void xargs(char *arg)
{
    printf("%s\n" ,arg);
    return ;
}
int 
main(int argc, char *argv[])
{
    int i=1;
    if(argc < 2){
        printf("Xars: Arguments reqired!\n");
        exit(0);
    }
    if(!strcmp(argv[i], "-n"))
    {
        i++;
        
    }
    for(i;i<argc;i++)
        xargs(argv[i]);
    exit(0);
}