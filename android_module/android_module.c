#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/highmem.h>

#include "../common/prinfo.h"
#include "gen.h"

MODULE_LICENSE("GPL");

#define __NR_ptreecall 384
unsigned long *sys_call_table = (unsigned long *) sys_call_table_addr;
// unsigned long *ia32_sys_call_table = (unsigned long *) 0xffffffff81803d80;

int make_rw(unsigned long address)
{
  /*
	unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	if(pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;
  */
	return 0;
}

int make_ro(unsigned long address)
{
  /*
	unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
  */
	return 0;
}

static int (*oldcall)(void);

int task_to_prinfo(struct task_struct* task, struct prinfo* __user buf) {
  struct prinfo info;
  struct list_head *child_task, *sibling_task;

  info.parent_pid = task->parent->pid; // TODO: init?
  info.pid = task->pid;

  child_task = &(task->children);
  
  if (list_empty(child_task))
    info.first_child_pid = 0;
  else
    info.first_child_pid = list_entry(child_task->next, struct task_struct, sibling)->pid;

  sibling_task = &(task->sibling);
  if (list_empty(sibling_task))
    info.next_sibling_pid = 0;
  else
    info.next_sibling_pid = list_entry(sibling_task->next, struct task_struct, sibling)->pid;
  
  info.state = task->state;
  info.uid = task->cred->uid;

  get_task_comm(info.comm, task);
  printk("%s\n", info.comm);

  copy_to_user(buf, &info, sizeof(struct prinfo));

  return 0;
}

int sys_ptreecall_iterate(struct prinfo __user *buf, int nr) {
  int N = nr, i = 0;
  struct task_struct *task;

  for_each_process(task)
  {
    task_to_prinfo(task, &buf[i]);
    ++i;
    if (i == N) return N;
  }
  return i;
}

asmlinkage int sys_ptreecall(struct prinfo __user *buf, int __user *nr)
{
  int user_nr;
  int new_nr;
  get_user(user_nr, nr);
  new_nr = sys_ptreecall_iterate(buf, user_nr);
  put_user(new_nr, nr);
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
