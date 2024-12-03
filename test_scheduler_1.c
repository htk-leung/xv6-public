// test4.c this test behavior of scheduler
//  First test for priority scheduler
#include "types.h"
#include "stat.h"
#include "user.h"

unsigned int MAX_PRIME = 1009;

int sqrt(int x);
void printPrimes(int pid);

int main()
{
    int i, pid = getpid();
    changenice(pid, 1);
    printf(1, "PID %d has a nice value of %d\n", pid + 1, 5);
    printf(1, "PID %d has a nice value of %d\n", pid + 2, 4);
    printf(1, "PID %d has a nice value of %d\n", pid + 3, 3);
    printf(1, "PID %d has a nice value of %d\n", pid + 4, 2);
    printf(1, "PID %d has a nice value of %d\n", pid + 5, 1);
    for (i = 5; i >= 1; i--)
    {
        if (fork() == 0)
        {
            pid = getpid();
            changenice(pid, i);
            printPrimes(pid);
            exit();
        }
    }
    for (i = 0; i < 5; i++)
        wait();
    exit();
}

void printPrimes(int pid)
{
    unsigned int i, j, prime;
    for (i = 2; i <= MAX_PRIME; i++)
    {
        prime = 1;
        for (j = 2; j <= sqrt(i); j++)
            if (i % j == 0)
            {
                prime = 0;
                break;
            }
        if (prime)
            printf(1, "%d: PID %d\n", i, pid);
    }
}
int sqrt(int x)
{
    int start = 1, end = x, result = -1;
    if (x < 0)
        return -1;
    if (x < 2)
        return x;
    while (start <= end)
    {
        int mid = (start + end) / 2;
        // x / mid is used to account for integer value that is close to a float sqrt
        if (mid == x / mid)
            return mid;
        if (mid < x / mid)
        {
            start = mid + 1;
            result = mid;
        }
        else
        {
            end = mid - 1;
        }
    }
    return result;
}

// Write up
/*
    The system calls changenice and getnice appear to be working fine.
    I haven't written a test for nice.c, but it appears to be working.
    Since the modified scheduler is a standard priority scheduler, it
    should behave as described in the following scenario.
    SCENARIO:
        Two user processes exist. One with nice value 1 and PID 1 and another with nice
        value 2 and PID 2. The CPU is currently running PID 2.
        A clock interupt is triggered. This results in the context being switched to the
        scheduler. The scheduler sees that PID 1 has the highest priority value and is runnable,
        so it switches context to PID 1.
        A clock interupt is triggered. This again results in the context scheduelr being switched
        to the scheduler. Since clock interupts transition a process from the running state to the
        runnable state, the scheduler again sees that PID 1 is runnable and has a higher priority value.
        The scheduler then switches context to PID 1 again. This continues until PID 1 exits.
        While PID 1 is running, PID 2 will be starved of runtime. The extra credit will be handling a
        starving process that has a lock to a resource that a higher priority process needs.

        I would expect test4.c to print all the primes for the high priority process first. This doesn't happen
        however. I believe this  is because printf() eventually leads to the write() system call which
        is a blocking system call. Consequently, when the highest priority process blocks due to a call to
        write, it can be in the blocke state long enough for a lower priority process to get picked by the scheduler.

        test4 alternates between printing primes for the highest priority process and the next highest priroity process.
        I believe that to be consistent to a prioity scheduler and processes that make blocking system calls
        to write. I need to talk with the TAs about this in OH.
*/