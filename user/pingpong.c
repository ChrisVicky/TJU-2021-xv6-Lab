#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int 
main(int argc, char *argv)
{
    int child_id;
    int parent_fb[2], child_fb[2];
    /* NOTES:   fd[0] is set up for reading, 
                fd[1] is set up for writing*/
    char child_buffer[80],parent_buffer[80];
    if(pipe(parent_fb)==-1 || pipe(child_fb)==-1){
        fprintf(2, "Creating pipes failed!\n");
        exit(1);
    }
    
    if((child_id = fork())==-1){
        fprintf(1, "Creating children Process Error!\n");
        exit(1);
    }
    if(child_id>0){
        /* Parent process */
        /* write to his child through fb[1] */
        write(child_fb[1], "ping", strlen("ping"));
        /* read from child through fb[0] */
        read(parent_fb[0], parent_buffer, sizeof(parent_buffer));
        printf("%d: received pong\n" ,getpid());
    }else{
        /* Children process */
        read(child_fb[0], child_buffer, sizeof(child_buffer));
        printf("%d: received ping\n" ,getpid());
        write(parent_fb[1], "pong", strlen("pong"));
    }
    exit(0);
}
