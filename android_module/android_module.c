#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/highmem.h>

#include "../common/prinfo.h"
#include "gen.h"

MODULE_LICENSE("GPL");

#define __NR_ptreecall 383
unsigned long *sys_call_table = (unsigned long *) sys_call_table_addr;

static int (*oldcall)(void);

/// @param task: task to save
/// @param buf: user buf for saving prinfo
int task_to_prinfo(struct task_struct* task, struct prinfo* __user buf) {
  struct prinfo info; // temporary storage for task info
  struct list_head *child_task, *sibling_task;
  int ret;

  info.parent_pid = task->parent->pid;
  info.pid = task->pid;

  child_task = &(task->children);
  
  if (list_empty(child_task))
    info.first_child_pid = 0;
  else
    info.first_child_pid = list_first_entry(child_task, struct task_struct, sibling)->pid;

  sibling_task = &(task->sibling);
  if (list_empty(sibling_task))
    info.next_sibling_pid = 0;
  else
    info.next_sibling_pid = list_first_entry(sibling_task, struct task_struct, sibling)->pid;
  
  info.state = task->state;
  info.uid = task->cred->uid;

  get_task_comm(info.comm, task);

  if ((ret = copy_to_user(buf, &info, sizeof(struct prinfo))) != 0) {
    printk(KERN_INFO "bad buffer addr %p, ret=%d\n", buf, ret);
    return -EFAULT;
  }

  return 0;
}

/// @param buf: user buf for saving information
/// @param task: root of iteration task
/// @param N: maximum elements allowed
/// @return elements saved into buf
static int ptree_dfs(struct prinfo __user *buf, struct task_struct *task, int N) {
  struct list_head *child_tasks = &(task->children), *pos;
  struct task_struct *child_task = NULL;
  int i = 1;
  int sz = 0;

  // if there's no more space, just return
  if (N <= 0) return 0;

  // save current task to buf
  if (task_to_prinfo(task, buf)) return -EFAULT;

  // if the task has no children, then only 1 element has been saved
  if (list_empty(child_tasks)) return 1;

  // iterate through child tasks
  list_for_each(pos, child_tasks) {
    child_task = list_entry(pos, struct task_struct, sibling);

    // iterate children
    sz = ptree_dfs(buf + i, child_task, N - i);

    // if something goes wrong, fault
    if (sz < 0) return -EFAULT;
    i += sz;
  }

  // return number of elements written
  return i;
}

static int sys_ptreecall_iterate(struct prinfo __user *buf, int nr) {
  return ptree_dfs(buf, &init_task, nr);
}

asmlinkage int sys_ptreecall(struct prinfo __user *buf, int __user *nr) {
  int user_nr;
  int new_nr;

  // get user nr from *nr
  if (get_user(user_nr, nr)) {
    printk(KERN_INFO "bad nr addr\n");
    return -EFAULT;
  }

  // save nr temporarily
  read_lock(&tasklist_lock);
  new_nr = sys_ptreecall_iterate(buf, user_nr);
  read_unlock(&tasklist_lock);

  // if there's something wrong, fault
  if (new_nr < 0) return -EFAULT;

  // save nr to *nr
  if (put_user(new_nr, nr)) return -EFAULT;

  return 0;
}

int init_module(void) {
  oldcall = (int(*)(void))(sys_call_table[__NR_ptreecall]);
  sys_call_table[__NR_ptreecall] = (unsigned long) sys_ptreecall;
  
  printk(KERN_INFO "pstree module loaded %p\n", sys_call_table);
  return 0;
}

void cleanup_module(void) {
  sys_call_table[__NR_ptreecall] = (unsigned long) oldcall;

  printk(KERN_INFO "pstree module unloaded\n");
}
