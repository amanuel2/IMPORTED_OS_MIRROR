		.global task_idle

.section .text

	task_idle:
		sti
		hlt
		cli
		ret
