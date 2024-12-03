#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    // printf(1, "In strace.c\n");

    // only valid uses : strace on / strace off
    // check if there are 2 arguments
    // otherwise print error

    if (argc != 2)
    {
        printf(2, "Usage : strace on OR strace off\n");
        exit();
    }
    
    int on = strcmp(argv[1], "on"); // equal = 0!
    int off = strcmp(argv[1], "off");

    if (!(on == 0) && !(off == 0))
    {
        printf(2, "Usage : strace on OR strace off\n");
        exit();
    }
    if (on == 0)
    {
        int output = 0;
        if((output = straceon()) < 0)
        {
            printf(2, "Cannot change p->strace\n");
            exit();
        }
        printf(2, "p->strace = %d\n", output);
    }
    else
    {
        int output = 0;
        if((output = straceoff()) < 0)
        {
            printf(2, "Cannot change p->strace\n");
            exit();
        }
        printf(2, "p->strace = %d\n", output);
    }
    exit();
}