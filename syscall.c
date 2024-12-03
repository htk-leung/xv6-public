#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc();

  if(addr >= curproc->sz || addr+4 > curproc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if(addr >= curproc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)curproc->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();
 
  if(argint(n, &i) < 0) // now value
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_changenice(void);
extern int sys_getnice(void);
extern int sys_printtable(void);
extern int sys_straceon(void);
extern int sys_straceoff(void);
extern int sys_check_strace(void);
extern int sys_set_proc_strace(void);

static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_changenice] sys_changenice,
[SYS_getnice] sys_getnice,
[SYS_printtable]  sys_printtable,
[SYS_straceon]  sys_straceon,
[SYS_straceoff]  sys_straceoff,
[SYS_check_strace] sys_check_strace,
[SYS_set_proc_strace] sys_set_proc_strace
};

static char *syscalls_strings[] = {
    [SYS_fork] = "fork",
    [SYS_exit] = "exit",
    [SYS_wait] = "wait",
    [SYS_pipe] = "pipe",
    [SYS_read] = "read",
    [SYS_kill] = "kill",
    [SYS_exec] = "exec",
    [SYS_fstat] = "fstat",
    [SYS_chdir] = "chdir",
    [SYS_dup] = "dup",
    [SYS_getpid] = "getpid",
    [SYS_sbrk] = "sbrk",
    [SYS_sleep] = "sleep",
    [SYS_uptime] = "uptime",
    [SYS_open] = "open",
    [SYS_write] = "write",
    [SYS_mknod] = "mknod",
    [SYS_unlink] = "unlink",
    [SYS_link] = "link",
    [SYS_mkdir] = "mkdir",
    [SYS_close] = "close",
    [SYS_straceon] = "trace_on",
    [SYS_straceoff] = "trace_off",
    [SYS_check_strace] = "check_strace",
    [SYS_set_proc_strace] = "set_proc_strace",
};

// void
// syscall(void)
// {
//   int num;
//   struct proc *curproc = myproc();

//   num = curproc->tf->eax;
//   if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
//     curproc->tf->eax = syscalls[num]();
//   } else {
//     cprintf("%d %s: unknown sys call %d\n",
//             curproc->pid, curproc->name, num);
//     curproc->tf->eax = -1;
//   }
// }

void syscall(void)
{
  int num;
  struct proc *proc = myproc();

  char *sys_call_trace;
  num = proc->tf->eax;

  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) // Will also need to verify that syscalls_strings[num] is valid.
  {

    // Special trace line for exit system call
    if (proc->strace != 0 && num == SYS_exit)
    {
      // *sys_call_trace = "TRACE: pid = %d | command_name = %s | syscall = %s\n", proc->pid, proc->name, syscalls_strings[num];
      if (N > 0) // avoid division by 0
                 // recent_system_calls[next_rc_sys_call_index++ % N] = sys_call_trace;
        cprintf("TRACE: pid = %d | command_name = %s | syscall = %s\n", proc->pid, proc->name, syscalls_strings[num]);
      itoa(5);
    }
    proc->tf->eax = syscalls[num]();
    // Standard trace line
    if (proc->strace != 0)
    {
      // *sys_call_trace = "TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n", proc->pid, proc->name, syscalls_strings[num], proc->tf->eax;
      if (N > 0) // avoid division by 0
                 // recent_system_calls[next_rc_sys_call_index++ % N] = sys_call_trace;
        cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n", proc->pid, proc->name, syscalls_strings[num], proc->tf->eax);
    }
  }
  else
  {
    cprintf("%d %s: unknown sys call %d\n",
            proc->pid, proc->name, num);
    proc->tf->eax = -1;
  }
}
