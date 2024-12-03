#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// get nice <pid> <value> or nice <value>
// return PID and old nice value
int
sys_changenice(void)
{
  int pid; 
  if(argint(0, &pid) < 0)
    return -1;

  int val; 
  if(argint(1, &val)< 0)
    return -1;

  return changenice(pid, val);
}

int sys_getnice(void)
{
    // cprintf("Inside sys_getnice\n");
    int pid;
    if (argint(0, &pid) < 0)
        return -1;
    return getnice(pid);
}

int
sys_printtable(void)
{
  return printtable();
}

int sys_straceon(void)
{
  return straceon();
}
int sys_straceoff(void)
{
  return straceoff();
}
int sys_check_strace()
{
  return check_strace();
}
int sys_set_proc_strace()
{
  return set_proc_strace();
}