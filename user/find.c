#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *target_file);

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(2, "error:You need pass in only 2 arguments\n");
        exit(1);
    }
    char *target_path = argv[1];
    char *target_file = argv[2];
    find(target_path, target_file);
    exit(0);
    return 0;
}

void find(char *path, char *target_file)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch (st.type)
    {
    case T_FILE:
        fprintf(2, "Usage: find dir file\n"); //应输入文件夹
        exit(1);
        break;
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            //不能进入.和..
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ); //文件夹下的文件名加入
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_DIR)
            {
                find(buf, target_file);
            }
            else if (st.type == T_FILE)
            {
                if (strcmp(de.name, target_file) == 0)
                {
                    printf("%s\n", buf);
                }
            }
        }
        break;
    }
    close(fd);
}