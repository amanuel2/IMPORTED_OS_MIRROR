		.global memset

.section .text

	memset:
		/* Push used registers. */
		pushl %esi
		pushl %edi
		/* Get parameters. */
		movl 12(%esp), %edi /* Destination. */
		movb 16(%esp), %al /* Byte value. */
		movl 20(%esp), %ecx /* Byte count. */
		/* Check if nothing to store. */
		cmpl $0, %ecx
		je .Lend
		/* Store at ascending order. */
		cld
		rep stosb
	.Lend:
		movl 12(%esp), %eax
		popl %edi
		popl %esi
		ret
