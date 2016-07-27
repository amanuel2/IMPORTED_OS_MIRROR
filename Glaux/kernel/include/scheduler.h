#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

#define TIME_QUANTUM 100

extern struct task_info* first_task;
extern struct task_info* last_task;
extern struct task_info* current_task;

void scheduler_init (void);
void scheduler_time (void);
void scheduler_run (bool save_old);
void scheduler_add_task (struct task_info*);
void scheduler_remove_current_task (void);
void scheduler_clean_finished_tasks (void);

#endif
