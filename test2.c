#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


int 
main(int argc, char **argv) 
{
    int pid;
    int loops = atoi(argv[1]);

    for (int i = 0; i < loops; i++)
    {
        pid = fork();
        if(pid < 0)
        {
            printf(1, "fork failed\n");
            exit();
        }

        if(pid == 0)
        {
            printf(1, "*");
            exit();
        }
        else wait();
    }
    printf(1, "\n");
    exit();
}