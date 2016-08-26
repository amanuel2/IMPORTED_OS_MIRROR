#include "scheduler.h"
#include "task.h"
#include "vmm.h"

struct task_info* task_create (size_t stacksize, void (*entry) (void))
{
	struct task_info* info = vmm_alloc (sizeof (struct task_info), 3);
	if (!info)
	{
		return NULL;
	}
	uintptr_t stackbase = (uintptr_t) vmm_alloc (stacksize, 3);
	if (!stackbase)
	{
		vmm_free (sizeof (struct task_info), info);
		return NULL;
	}
	struct task_stack* stackptr = (struct task_stack*) (uintptr_t)
	                                 (stackbase + stacksize -
	                                  sizeof (struct task_stack));
	info->stackptr = (uintptr_t) stackptr;
	info->stackbase = stackbase;
	info->stacksize = stacksize;
	info->sleeptime = 0;
	info->exectime = TIME_QUANTUM;
	info->sleeping = false;
	stackptr->eip = (uint32_t) entry;
	scheduler_add_task (info);
	return info;
}

void task_sleep_current (uint32_t msec)
{
	current_task->sleeping = true;
	current_task->sleeptime = msec;
	scheduler_run (true);
}

void task_finish_current (void)
{
	scheduler_remove_current_task ();
}
