#include <stdbool.h>
#include <stddef.h>

#include "lfb.h"
#include "log.h"
#include "mboot.h"
#include "mod.h"
#include "paddr.h"
#include "panic.h"
#include "pmm.h"
#include "ps2.h"
#include "scheduler.h"
#include "task.h"
#include "vbe.h"
#include "vmm.h"

struct mboot_info* mboot_info;
struct vbe_ctrl_info* vbe_ctrl_info;
struct vbe_mode_info* vbe_mode_info;
struct mod_ent* mod_ent;
struct mmap_ent* mmap_ent;

void task_test_2 (void)
{
	log_printf ("B");
	task_create (16384, task_test_2);
	task_finish_current ();
}

void task_test_1 (void)
{
	log_printf ("A");
	task_create (16384, task_test_2);
	task_finish_current ();
}

void kernel (void)
{
	if (!ps2_init ())
	{
		log_printf ("Warning: PS/2 initialisation failed\n");
	}
	log_printf ("The Glaux Operating System Project version 0.0.1-dev\n");
	task_create (16384, task_test_1);
	while (true)
	{
		task_sleep_current (5000);
	}
}

void main (paddr_t mboot)
{
	mboot_info = vmm_map_initial (sizeof (struct mboot_info),
	                              mboot, 1);
	mod_ent = vmm_map_initial (sizeof (struct mod_ent),
	                           mboot_info->mods_addr, 1);
	mmap_ent = vmm_map_initial (mboot_info->mmap_length,
	                            mboot_info->mmap_addr, 1);
	vbe_ctrl_info = vmm_map_initial (sizeof (struct vbe_ctrl_info),
	                                 mboot_info->vbe_ctrl_info, 1);
	vbe_mode_info = vmm_map_initial (sizeof (struct vbe_mode_info),
	                                 mboot_info->vbe_mode_info, 1);
	pmm_list ();
	mod_scan ();
	lfb_init ();
	if (!task_create (16384, kernel))
	{
		panic ("Panic: Cannot create main kernel thread.\n");
	}
	if (!task_create (16384, scheduler_clean_finished_tasks))
	{
		panic ("Panic: Cannot create task cleaner thread.\n");
	}
	scheduler_init ();
}
