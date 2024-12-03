#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    printf(1, "In nice.c\n");
    int pid, val, oldNice;
    if (argc == 2)
    {
        pid = getpid();
        val = atoi(argv[1]);
    }
    else if (argc == 3)
    {
        pid = atoi(argv[1]);
        val = atoi(argv[2]);
    }
    else
    {
        printf(2, "Invalid parameters for nice()\n");
        exit();
    }
    if (val < 1 || val > 5)
    {
        printf(2, "Range for nice values is [1, 5]\n");
        exit();
    }
    oldNice = getnice(pid);
    changenice(pid, val);
    printf(1, "%d %d\n", pid, oldNice);
    exit();
}