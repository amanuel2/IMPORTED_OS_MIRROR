		.global task_switch

.section .text

	task_switch:
		pushfl
		pushl %ebp
		pushl %edi
		pushl %esi
		pushl %edx
		pushl %ecx
		pushl %ebx
		pushl %eax

		movl 40(%esp), %eax
		movl %esp, 0(%eax)

		movl 36(%esp), %eax
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
