#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    // printf(1, "In strace.c\n");

    // before running, check flag status to reset unwanted ones

    // only valid uses : strace on / strace off
    // check if there are 2 arguments
    // otherwise print error
    if (argc < 2)
    {
        printf(2, "Usage : strace on, strace off, or strace run <command>\n");
        exit();
    }
    int on = strcmp(argv[1], "on"); // equal = 0!
    int off = strcmp(argv[1], "off");

    if (on == 0)
        straceon();
    else if (off == 0)
        straceoff();
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
    else if (strcmp(argv[1], "dump") == 0)
        strace_dump();
    else if(strcmp(argv[1], "-e") == 0) 
    {
        // check for correct input
        if(argc < 3)
        {
            printf(2, "Usage : no system call specified with flag -e\n");
            exit();
        }
        // strace_selprint();
        strace_selon(argc, argv[2]);
        // strace_seloff();
        // strace_selprint();
    }
    exit();
}

// strace -e ...                                        << strace.c
// CHECKS A, SEEs nothing                               << strace_status
// 1st command SETs A                                   << strace_selon
// rest of system calls will SEE A but do nothing       << write, read...

// echo hello                                           << ALL user programs
// 2nd command SEEs A, UNSET A, SET B                   << strace_status
// rest of system calls SEE B and print accordingly     << write, read...

// 
// 3rd command SEEs B, UNSET B                          << strace_status