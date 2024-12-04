#include "types.h"
#include "stat.h"
#include "user.h"

#define NUM_CHILDREN 7
// #define COUNT_TO 1000000000
int main()
{
    int i, pids[NUM_CHILDREN];
    for(i = 0; i < NUM_CHILDREN; i++)
        if((pids[i] = fork()) == 0)
        {
            printf(1, "Child %d\n", i);
            proc_strace_dump(getpid());
            // //keep children busy
            // for(j = 0; j < COUNT_TO; j++)
            //     continue;
            exit();
        }
    for(i = 0; i < NUM_CHILDREN; i++)
        wait();
    exit();
}