#include "../common/prinfo.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/highmem.h>

MODULE_LICENSE("GPL");

#define __NR_ptreecall 100
unsigned long *sys_call_table = (unsigned long *) 0xffffffff81800200;
unsigned long *ia32_sys_call_table = (unsigned long *) 0xffffffff81803d80;

int make_rw(unsigned long address)
{
	unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	if(pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;
	return 0;
}

int make_ro(unsigned long address)
{
	unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
	return 0;
}

static int (*oldcall)(void);

asmlinkage int sys_ptreecall(struct prinfo *buf, int* nr)
{
  struct task_struct *task;
  printk("In syscall!");
  
  for_each_process(task)
  {
    printk("%s [%d]\n", task->comm, task->pid);
  }

  return 0;
}

int init_module(void)
{
  make_rw((unsigned long) sys_call_table);

  oldcall = (int(*)(void))(sys_call_table[__NR_ptreecall]);
  sys_call_table[__NR_ptreecall] = (unsigned long) sys_ptreecall;

  make_ro((unsigned long) sys_call_table);
  
  printk(KERN_INFO "pstree module loaded %p\n", sys_call_table);
  return 0;
}

void cleanup_module(void)
{
  make_rw((unsigned long) sys_call_table);

  sys_call_table[__NR_ptreecall] = (unsigned long) oldcall;

  make_ro((unsigned long) sys_call_table);

  printk(KERN_INFO "pstree module unloaded\n");
}
