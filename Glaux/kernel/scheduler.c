#include <stddef.h>

#include "scheduler.h"
#include "task.h"
#include "vmm.h"

struct task_info* first_task;
struct task_info* last_task;
struct task_info* current_task;

struct task_info* first_finished;
struct task_info* last_finished;

bool initialised = false;
bool locked = false;

void scheduler_init (void)
{
	initialised = true;
	current_task = first_task;
	task_start (current_task);
}

void scheduler_time (void)
{
	if (!initialised)
	{
		return;
	}
	if (locked)
	{
		while (locked)
		{
			;
		}
	}
	locked = true;
	struct task_info* task = first_task;
	while (task)
	{
		if (task->sleeping)
		{
			task->sleeptime -= 1;
			if (task->sleeptime == 0)
			{
				task->sleeping = false;
			}
		}
		task = task->next;
	}
	current_task->exectime -= 1;
	if (current_task->exectime == 0)
	{
		locked = false;
		scheduler_run (true);
	}
	locked = false;
}

void scheduler_run (bool save_old)
{
	if (!initialised)
	{
		return;
	}
	while (locked)
	{
		;
	}
	locked = true;
	struct task_info* prev = current_task;
	while (true)
	{
		if (current_task == last_task)
		{
			current_task = first_task;
		}
		else
		{
			current_task = current_task->next;
		}
		if (current_task->sleeping == false)
		{
			current_task->exectime = TIME_QUANTUM;
			locked = false;
			if (save_old)
			{
				task_switch (current_task, prev);
			}
			else
			{
				task_start (current_task);
			}
			return;
		}
		if (current_task == prev)
		{
			locked = false;
			task_idle ();
			locked = true;
		}
	}
	locked = false;
}

void scheduler_add_task (struct task_info* info)
{
	while (locked)
	{
		;
	}
	locked = true;
	if (!first_task)
	{
		first_task = info;
		first_task->prev = NULL;
		first_task->next = NULL;
		last_task = info;
	}
	else
	{
		struct task_info* prev_task = last_task;
		prev_task->next = info;
		last_task = last_task->next;
		last_task->prev = prev_task;
		last_task->next = NULL;
	}
	locked = false;
}

void scheduler_remove_current_task (void)
{
	while (locked)
	{
		;
	}
	locked = true;
	struct task_info* prev = current_task->prev;
	struct task_info* next = current_task->next;
	if (prev)
	{
		prev->next = next;
	}
	else
	{
		if (current_task == first_task)
		{
			first_task = next;
		}
	}
	if (next)
	{
		next->prev = prev;
	}
	else
	{
		if (current_task == last_task)
		{
			last_task = prev;
		}
	}
	if (!first_finished)
	{
		first_finished = current_task;
		first_finished->prev = NULL;
		first_finished->next = NULL;
		last_finished = current_task;
	}
	else
	{
		struct task_info* prev_finished = last_finished;
		prev_finished->next = current_task;
		last_finished = last_finished->next;
		last_finished->prev = prev_finished;
		last_finished->next = NULL;
	}
	current_task = prev ? prev : last_task;
	locked = false;
	scheduler_run (false);
}

void scheduler_clean_finished_tasks (void)
{
	while (true)
	{
		while (locked)
		{
			;
		}
		locked = true;
		while (first_finished)
		{
			struct task_info* next = first_finished->next;
			vmm_free (first_finished->stacksize,
			          (void*) first_finished->stackbase);
			vmm_free (sizeof (struct task_info), first_finished);
			first_finished = next;
		}
		last_finished = NULL;
		locked = false;
		task_sleep_current (100);
	}
}
