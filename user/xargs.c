#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char args[MAXARG][MAXARG];
    char *temp_args[MAXARG];
    /* Init */
    memset(args, 0, MAXARG * MAXARG);
    /* get input from argv */
    int i, j, len;
    for (i = 0; i < argc - 1; i++)
    {
        strcpy(args[i], argv[i + 1]);
    }
    /* 将以下 stdin 的部分转换成 参数成分 */
    j = 0;
    while ((len = read(0, &args[i][j], sizeof(char))) > 0)
    {
        if (args[i][j] == '\n')
        {
            args[i][j] = '\0';
            /* 一行读完了 */
            for (i = 0; i < MAXARG - 1; i++)
            {
                temp_args[i] = args[i];
            }
            temp_args[MAXARG - 1] = 0;
            /* execute */
            int id = fork();
            if (id == 0)
            {
                exec(temp_args[0], temp_args);
            }
            else
            {
                wait(&id);
            }
            /* 重置 */
            i = argc - 1;
            j = 0;
            memset(args[i], 0, (MAXARG - i) * MAXARG);
        }
        else if (args[i][j] == ' ')
        {
            args[i][j] = '\0';
            i++;
            j = 0;
            if (i == MAXARG)
            {
                fprintf(2, "Too many arguments, please make sure < 32\n");
                break;
            }
        }
        else
            j++;
    }
    exit(0);
}