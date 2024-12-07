#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    // only valid uses : strace on / strace off
    // check if there are 2 arguments
    // otherwise print error
    if (argc < 2)
    {
        printf(2, "Usage : strace on, strace off, or strace run <command>\n");
        exit();
    }
    if (argc == 2)
    {
        int on = strcmp(argv[1], "on"); // equal = 0!
        int off = strcmp(argv[1], "off");
        if (on == 0)
            straceon();
        else if (off == 0)
            straceoff();
        else if (strcmp(argv[1], "dump") == 0)
            strace_dump();
    }

    if (argc >= 2)
    {
        // flags
        int flag = 0;
        for (int i = 1; i < argc; i++)
        {
            if(argv[i][0] == '-')
                flag = 1;
        }
        if (flag)
        {
            strace_selon(argc, argv);
        }
        // strace run <command>
        // OH question: Does strace run <command> have to be implemented in shell or can it be implemented entirely within strace.c?
        else if (strcmp(argv[1], "run") == 0)
        {
            if (fork() == 0)
            {
                char **newArgv = &(argv[2]);
                set_proc_strace();
                exec(argv[2], newArgv);
                printf(1, "Exec failed for strace<command>\n");
            }
            wait();
        }
    }
    exit();
}


