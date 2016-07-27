		.global task_start

.section .text

	task_start:
		movl 4(%esp), %eax
		movl 0(%eax), %esp

		popl %eax
		popl %ebx
		popl %ecx
		popl %edx
		popl %esi
		popl %edi
		popl %ebp
		popfl
		sti
		ret
