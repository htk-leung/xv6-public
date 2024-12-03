//  Second test for priority scheduler
#include "types.h"
#include "stat.h"
#include "user.h"

#define LOOPS 2000
#define PCOUNT 5

int main()
{
    int i, pid = getpid();

    for (i = PCOUNT; i >= 1; i--)
    {
        if (fork() == 0)
        {
            // print table to see current status
            printtable();

            // demonstrate in child process, show initial pid and nice value
            pid = getpid();
            int oldnice = getnice(pid);
            printf(1, "pid : %d\told priority : %d\n", pid, oldnice);
            changenice(pid, i);

            int z;
            // int x;
            for (z = 0; z < LOOPS; z++)
                printf(1, "%d ", pid);
            // x = x + 3.14*89.64;

            // print table to see current status
            printf(1, "pid : %d\tcomplete\n", pid);
            printtable();

            exit();
        }
    }
    for (i = 0; i < PCOUNT; i++)
        wait();
    exit();
}