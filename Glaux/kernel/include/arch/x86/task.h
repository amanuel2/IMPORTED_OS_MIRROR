#ifndef TASK_H
#define TASK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct task_info
{
	uint32_t stackptr;
	uint32_t stackbase;
	uint32_t stacksize;
	uint32_t sleeptime;
	uint32_t exectime;
	bool sleeping;
	struct task_info* prev;
	struct task_info* next;
};

struct task_stack
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t eflags;
	uint32_t eip;
};

struct task_info* task_create (size_t, void (*) (void));
void task_sleep_current (uint32_t);
void task_finish_current (void);
void task_start (struct task_info*);
void task_switch (struct task_info*, struct task_info*);
void task_idle (void);

#endif
