		.global halt

.section .text

	halt:
		cli
		hlt
		jmp halt

