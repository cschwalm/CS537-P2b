#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"
#include "sysfunc.h"

//extern struct ptable;

extern struct pstat stats;

int
sys_reserve(void)
{

	int percent;
	
  if(argint(0, &percent) < 0)
    return -1;

	if (percent < 1 || percent > 100)
		return -1;

	if (ticketCount + percent > 200)
		return -1;

	if (ticketCount == -1)
		ticketCount = percent - 1;
	else
		ticketCount += (percent - 1); //Percent is 1 to 100
	proc->percent = (percent - 1);
	proc->bid = 0;

	return 0;
}

int
sys_spot(void)
{
	
	int bid;
	
  if(argint(0, &bid) < 0)
    return -1;

	if (bid < 0)
		return -1;

	proc->percent = 0;
	proc->bid = bid;

	return 0;
}

int
sys_getpinfo(void)
{

	struct pstat* stat;
	
  if(argptr(0, (char**) &stat, sizeof(stats) < 0))
    return -1;

	//getpinfo(stat);
	stat = &stats;

	return 0;
}

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
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
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
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
