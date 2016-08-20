		.global inport8
		.global inport16
		.global inport32

		.global outport8
		.global outport16
		.global outport32

.section .text

	inport8:
		movl 4(%esp), %edx
		inb %dx, %al
		andl $0xFF, %eax
		ret

	inport16:
		movl 4(%esp), %edx
		inw %dx, %ax
		andl $0xFFFF, %eax
		ret

	inport32:
		movl 4(%esp), %edx
		inl %dx, %eax
		ret

	outport8:
		movl 4(%esp), %edx
		movl 8(%esp), %eax
		andl $0xFF, %eax
		outb %al, %dx
		ret

	outport16:
		movl 4(%esp), %edx
		movl 8(%esp), %eax
		andl $0xFFFF, %eax
		outw %ax, %dx
		ret

	outport32:
		movl 4(%esp), %edx
		movl 8(%esp), %eax
		outl %eax, %dx
		ret
