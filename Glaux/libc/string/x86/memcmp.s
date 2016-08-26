		.global memcmp

.section .text

	memcmp:
		/* Push used registers. */
		pushl %ebx
		pushl %esi
		pushl %edi
		/* Get parameters. */
		mov 16(%esp), %edi /* Destination. */
		mov 20(%esp), %esi /* Source. */
		mov 24(%esp), %ecx /* Byte count. */
		/* Check if nothing to compare. */
		cmpl $0, %ecx
		je .Lequal
		/* Compare at ascending order. */
		cld
		repe cmpsb
		/* Check result. */
		jb .Lbelow
		ja .Labove
		jmp .Lequal
	.Lbelow:
		movl $-1, %eax
		jmp .Lend
	.Labove:
		movl $1, %eax
		jmp .Lend
	.Lequal:
		xorl %eax, %eax
	.Lend:
		popl %edi
		popl %esi
		popl %ebx
		ret
