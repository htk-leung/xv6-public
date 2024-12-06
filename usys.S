#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
SYSCALL(changenice)
SYSCALL(getnice)
SYSCALL(printtable)
SYSCALL(straceon)
SYSCALL(straceoff)
SYSCALL(check_strace)
SYSCALL(set_proc_strace)
SYSCALL(strace_dump)
SYSCALL(strace_selon)
SYSCALL(strace_seloff)
SYSCALL(strace_selprint)
SYSCALL(strace_selstatus)