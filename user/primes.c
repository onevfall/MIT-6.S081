#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READEND 0
#define WRITEEND 1

void makeChild(int *pleft);

int main(int argc, char *argv[])
{
    int p[2];
    pipe(p); //创建管道

    if (fork() == 0)
    { //子进程
        makeChild(p);
    }
    else
    { //主进程
        close(p[READEND]);
        for (int i = 2; i <= 35; i++)
        {
            write(p[WRITEEND], &i, sizeof(int));
        }
        close(p[WRITEEND]);
        wait((int *)0);
    }
    exit(0);
    return 0;
}

void makeChild(int *pleft)
{
    int pright[2];
    int first;
    close(pleft[WRITEEND]);

    int isReadEnd = read(pleft[READEND], &first, sizeof(int));
    if (isReadEnd == 0)
    { //第一个值为0，到达终点
        exit(0);
    }
    else
    {
        printf("prime %d \n", first);
        pipe(pright); //创建右pipe
        if (fork() == 0)
        {
            makeChild(pright);
        }
        else
        {
            for (;;)
            {
                int num;
                close(pright[READEND]);
                if (read(pleft[READEND], &num, sizeof(int)) == 0)
                    break;
                else
                {
                    if (num % first != 0)
                    { //并非其倍数才能保留
                        write(pright[WRITEEND], &num, sizeof(int));
                    }
                }
            }
            close(pright[WRITEEND]);
            close(pleft[READEND]);
            wait((int *)0);
        }
    }
    exit(0);
}