// First test for priority scheduler
#include "types.h"
#include "stat.h"
#include "user.h"

unsigned int MAX = 3000;

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
    for (i = 2; i <= MAX; i++)
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