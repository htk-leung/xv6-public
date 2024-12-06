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
 
  if(argint(n, &i) < 0)
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

// Adds int to string.
int unsignedIntToString(int n, char *str)
{
  int i = 0, j, l;
  if (n == 0)
  {
    str[i] = '0';
    return 1;
  }
  char reversed[10];
  while (n > 0)
  {
    reversed[i++] = (n % 10) + '0';
    n = n / 10;
  }
  l = i;
  reversed[i] = 0;
  for (j = 0; j < l; j++)
    str[j] = reversed[--i];
  str[j] = 0;
  return l;
}
// string formatter loosely based on cprintf
void strFormatter(char *fmt, char *result, int pid, char *cmd, char *syscall, int returnVal)
{
  int i, c, l = 0, j = 0;
  char *s;
  if (fmt == 0)
    panic("null fmt in snprintf");

  for (i = 0; (c = fmt[i] & 0xff) != 0; i++)
  {
    if (j > 3 || (returnVal == -1 && j > 2))
      break;
    if (c != '%')
    {
      result[l++] = (c);
      continue;
    }
    c = fmt[++i] & 0xff;
    switch (j)
    {
    case 0:
      l += unsignedIntToString(pid, &(result[l]));
      break;
    case 1:
      s = cmd;
      for (; *s; s++)
        result[l++] = *s;
      break;
    case 2:
      s = syscall;
      for (; *s; s++)
        result[l++] = *s;
      break;
    case 3:
      l += unsignedIntToString(returnVal, &(result[l]));
      break;
    }
    j++;
  }
  result[l] = '\0';
}

// Array size of stored system call events
#define X 10
// Size of buffer for each stored system call event
#define TRACE_LOG_BUFFER_SIZE 256

// Create array in kernel memory to store system call log
char system_call_log[X][TRACE_LOG_BUFFER_SIZE];
// Create circular index for system call log
unsigned int next_rc_sys_call_index = 0;

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
extern int sys_lock(void);
extern int sys_resourcerelease(void);
extern int sys_lockstate(void);
extern int sys_pilock(void);
extern int sys_printtable(void);
extern int sys_straceon(void);
extern int sys_straceoff(void);
extern int sys_check_strace(void);
extern int sys_set_proc_strace(void);
extern int sys_strace_dump(void);
extern int sys_strace_selon(void);
extern int sys_strace_seloff(void);
extern int sys_strace_selprint(void);
extern int sys_strace_selstatus(void);

static int (*syscalls[])(void) = {
    [SYS_fork] sys_fork,
    [SYS_exit] sys_exit,
    [SYS_wait] sys_wait,
    [SYS_pipe] sys_pipe,
    [SYS_read] sys_read,
    [SYS_kill] sys_kill,
    [SYS_exec] sys_exec,
    [SYS_fstat] sys_fstat,
    [SYS_chdir] sys_chdir,
    [SYS_dup] sys_dup,
    [SYS_getpid] sys_getpid,
    [SYS_sbrk] sys_sbrk,
    [SYS_sleep] sys_sleep,
    [SYS_uptime] sys_uptime,
    [SYS_open] sys_open,
    [SYS_write] sys_write,
    [SYS_mknod] sys_mknod,
    [SYS_unlink] sys_unlink,
    [SYS_link] sys_link,
    [SYS_mkdir] sys_mkdir,
    [SYS_close] sys_close,
    [SYS_straceon] sys_straceon,
    [SYS_straceoff] sys_straceoff,
    [SYS_check_strace] sys_check_strace,
    [SYS_set_proc_strace] sys_set_proc_strace,
    [SYS_strace_dump] sys_strace_dump,
    [SYS_strace_selon] sys_strace_selon,
    [SYS_strace_seloff] sys_strace_seloff,
    [SYS_strace_selprint] = sys_strace_selprint,
    [SYS_strace_selstatus] = sys_strace_selstatus
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
    [SYS_strace_dump] = "strace_dump",
    [SYS_strace_selon] = "strace_selon",
    [SYS_strace_seloff] = "strace_seloff",
    [SYS_strace_selprint] = "strace_selprint",
    [SYS_strace_selstatus] = "strace_selstatus"
};

void init_sys_call_log()
{
  int i;
  for (i = 0; i < X; i++)
    *system_call_log[i] = 0;
}

int strace_dump()
{
  int i;
  // Check if X system calls haven't been made yet
  if (system_call_log[next_rc_sys_call_index] == 0)
  {
    cprintf("In strace_dump if loop\n");
    for (i = 0; i < next_rc_sys_call_index; i++)
      cprintf("trace log %d = %s\n", i, system_call_log[i]);
  }
  else
  {
    cprintf("In strace_dump else loop\n");
    // Print first section of circular array
    for (i = next_rc_sys_call_index % X; i < X; i++)
      cprintf("trace log %d = %s\n", i, system_call_log[i]);
    // Print second section of circular array
    for (i = 0; i < next_rc_sys_call_index % X; i++)
      cprintf("trace log %d = %s\n", i, system_call_log[i]);
  }
  return 0;
}

int sys_strace_dump()
{
  return strace_dump();
}

void
syscall(void)
{
  int i, num, init_flag = 0;
  char init_str[5] = "init";
  struct proc *curproc = myproc();
  // Initialize system calls log during first system call made y init
  if (next_rc_sys_call_index == 0)
  {
    for (i = 0; i < 5; i++)
      if (curproc->name[i] != init_str[i])
        init_flag = 1;
    if (init_flag)
      init_sys_call_log();
  }
  num = curproc->tf->eax;
  // get -e setting for syscall
  int flagE = syscalls[SYS_strace_selprint](); // 0 = don't print, 1 = print

  // -e not working yet
  // check flagE value by printing
  // cprintf("flagE of %s = %d\n", syscalls_strings[num], flagE);

  // Will also need to verify that syscalls_strings[num] is valid.
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) 
  {
    // Special trace line for exit and exec system calls
    if ((num == SYS_exit || num == SYS_exec))
    {
      if (X > 0) // avoid division by 0
      // what does this do??
        strFormatter("TRACE: pid = %d | command_name = %s | syscall = %s\n", system_call_log[next_rc_sys_call_index++ % X],
                     curproc->pid, curproc->name, syscalls_strings[num], -1);
      if (curproc->strace != 0 || flagE == 1) // process flags here
        cprintf("TRACE: pid = %d | command_name = %s | syscall = %s\n", curproc->pid, curproc->name, syscalls_strings[num]);
    }
    curproc->tf->eax = syscalls[num]();
    // Standard trace line

    if (num != SYS_exit && num != SYS_exec)
    {
      if (X > 0) // avoid division by 0
        strFormatter("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n", system_call_log[next_rc_sys_call_index++ % X],
                     curproc->pid, curproc->name, syscalls_strings[num], curproc->tf->eax);
      if (curproc->strace != 0 || flagE == 1)
      {
        cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
                curproc->pid, curproc->name, syscalls_strings[num], curproc->tf->eax);
      }
    }
  } 
  else 
  {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}