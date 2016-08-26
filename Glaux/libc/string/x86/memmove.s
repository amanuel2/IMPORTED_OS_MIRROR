		.global memmove
		.global memcpy

.section .text

	memmove:
	memcpy:
		/* Push used registers. */
		pushl %ebx
		pushl %esi
		pushl %edi
		/* Get parameters. */
		movl 16(%esp), %edi /* Destination. */
		movl 20(%esp), %esi /* Source. */
		movl 24(%esp), %ecx /* Byte count. */
		/* Check if nothing to copy. */
		cmpl $0, %ecx
		je .Lend
		/* Check in what order should we copy to avoid overlap. */
		cmpl %edi, %esi
		ja .Lset_asc
		jb .Lset_desc
		/* Same source and destination address, no point in copying. */
		jmp .Lend
	.Lset_asc:
		cld
		rep movsb
		jmp .Lend
	.Lset_desc:
		addl %ecx, %esi
		addl %ecx, %edi
		subl $1, %esi
		subl $1, %edi
		std
		rep movsb
		cld
	.Lend:
		movl 16(%esp), %eax /* Destination. */
		popl %edi
		popl %esi
		popl %ebx
		ret
