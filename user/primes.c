#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
typedef struct node{
    int data;
    struct node * next;
};
int
main(int argc, char *argv)
{
    if(argc > 1){
        fprintf(2, "Too many arguments!\n");
        exit(1);
    }
    int i;
    int num[36];
    node * head;
    head = (node*) malloc(sizeof(node));
    for(i=1;i<=35;i++){
        node *temp = new node;
    }

    exit(0);
}
