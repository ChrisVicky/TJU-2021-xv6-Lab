#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if(argc == 1){
        fprintf(2, "Argument for sleep time required!\n");
        exit(1);
    }else if(argc == 2){
        sleep(atoi(argv[1]));
        exit(0);
    }else{
        fprintf(2, "Too many arguments!\n");
        exit(1);
    }
}