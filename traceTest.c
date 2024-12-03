#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    if (fork() != 0)
        wait();
    return 0;
}