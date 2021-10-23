#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

int strcmp_blank(char *buf, char *file_name)
{
    int i;
    for(i=0;i<strlen(file_name);i++){
        if(buf[i]!=file_name[i]) return 1;
    }
    return 0;
}

void 
find(char *path, char *file_name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    if(st.type==T_FILE){
        if(!strcmp_blank(fmtname(path), file_name)) 
            printf("%s%s\n", path, fmtname(path), st.type, st.ino, st.size);
    }else if(st.type==T_DIR){
        if (!strcmp_blank(fmtname(path), file_name))
        {
            printf("%s%s\n", path, fmtname(path));
        }
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("ls: path too long\n");
            close(fd);
            return ;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            if(!strcmp_blank(fmtname(buf), file_name)){
                printf("%s\n",buf);
            }
            if(!strcmp_blank(fmtname(buf), "..") || !strcmp_blank(fmtname(buf), ".")) continue;
            if(st.type==T_DIR){
                find(buf, file_name);
            }
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    int i;
    if (argc < 3)
    {
        printf("Arguments required!\n");
        exit(0);
    }
    for (i = 2; i < argc; i++)
    {
        find(argv[1], argv[i]);
    }
    exit(0);
}
