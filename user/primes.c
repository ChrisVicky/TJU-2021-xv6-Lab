#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int
main(int argc, char *argv)
{
    if(argc > 1){
        fprintf(2, "Too many arguments!\n");
        exit(1);
    }
    int fb[2];
    pipe(fb);
    int nxtfb[2];
    pipe(nxtfb);
    int i;
    for(i=2;i<40;i++){
        write(fb[1], &i, 1);
    }
    i = 0;
    write(fb[1], &i, 1);
    int id = fork();
    int count = 1;
    while(id==0 && count ){
        count = 0;
        int tp;
        read(fb[0], &tp, 1);
        int start = tp;
        if(start) count++;
        else exit(0);
        printf("prime: %d\n" ,start);
        while(tp){
            read(fb[0], &tp, 1);
            if((tp==0) || (tp % start)){
                write(nxtfb[1], &tp, 1);
                count ++;
            }
        }
        int temp0 = nxtfb[0], temp1 = nxtfb[1];
        nxtfb[0] = fb[0], nxtfb[1] = fb[1];
        fb[0] = temp0, fb[1] = temp1;
        if(count) id = fork();
        if(id) exit(0);
    }
    if(id){
        wait(&id);
        printf("end\n");
        exit(0);
    }
    exit(0);
}
