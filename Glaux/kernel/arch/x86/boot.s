		.global _start
		.global idt_start

.section .mboot

	mboot_header:
		.align 4
		.long 0x1BADB002
		.long 0x00000007
		.long - (0x1BADB002 + 0x00000007)
		.long 0
		.long 0
		.long 0
		.long 0
		.long 0
		.long 0
		.long 800
		.long 600
		.long 32

.section .text

	_start:
		movl $(pagetabs - 0xE0000000 + 4), %edi
		movl $0x1000, %esi
		movl $4095, %ecx

	add_ent:
		cmpl $(_text_start - 0xE0000000), %esi
		jl outside_text
		cmpl $(_text_end - 0xE0000000), %esi
		jge outside_text
		orl $0x01, %esi;
		movl %esi, (%edi)
		jmp cont

	outside_text:
		cmpl $(_data_start - 0xE0000000), %esi
		jl outside_data
		cmpl $(_data_end - 0xE0000000), %esi
		jge outside_data
		orl $0x03, %esi;
		movl %esi, (%edi)
		jmp cont

	outside_data:
		cmpl $(_rodata_start - 0xE0000000), %esi
		jl outside_rodata
		cmpl $(_rodata_end - 0xE0000000), %esi
		jge outside_rodata
		orl $0x01, %esi;
		movl %esi, (%edi)
		jmp cont

	outside_rodata:
		cmpl $(_bss_start - 0xE0000000), %esi
		jl cont
		cmpl $(_bss_end - 0xE0000000), %esi
		jge cont
		orl $0x03, %esi;
		movl %esi, (%edi)

	cont:
		andl $(0xFFFFF000), %esi
		addl $0x1000, %esi
		addl $4, %edi
		loop add_ent

	break:
		movl $(pagetabs - 0xE0000000 + 0x0003), pagedir - 0xE0000000 + 0
		movl $(pagetabs - 0xE0000000 + 0x1003), pagedir - 0xE0000000 + 1*4
		movl $(pagetabs - 0xE0000000 + 0x2003), pagedir - 0xE0000000 + 2*4
		movl $(pagetabs - 0xE0000000 + 0x3003), pagedir - 0xE0000000 + 3*4
		movl $(pagetabs - 0xE0000000 + 0x0003), pagedir - 0xE0000000 + 896*4
		movl $(pagetabs - 0xE0000000 + 0x1003), pagedir - 0xE0000000 + 897*4
		movl $(pagetabs - 0xE0000000 + 0x2003), pagedir - 0xE0000000 + 898*4
		movl $(pagetabs - 0xE0000000 + 0x3003), pagedir - 0xE0000000 + 899*4
		movl $(pagedir - 0xE0000000 + 0x0003), pagedir - 0xE0000000 + 1023*4
		movl $(pagedir - 0xE0000000), %ecx
		movl %ecx, %cr3
		movl %cr0, %ecx
		orl $0x80010000, %ecx
		movl %ecx, %cr0
		lea higher_half, %ecx
		jmp *%ecx

	higher_half:
		movl $0, pagedir + 0
		movl $0, pagedir + 1*4
		movl $0, pagedir + 2*4
		movl $0, pagedir + 3*4
		movl %cr3, %ecx
		movl %ecx, %cr3
		movl $stack_top, %esp
		lgdt gdt_info
		ljmp $0x08, $after_lgdt

	after_lgdt:
		movl $0x10, %ecx
		movl %ecx, %ds
		movl %ecx, %es
		movl %ecx, %fs
		movl %ecx, %gs
		movl %ecx, %ss
		pushl %ebx
		call pic_init
		call pit_init
		call idt_init
		lidt idt_info
		sti
		call main
		jmp halt

.section .rodata

	idt_info:
		.word 4095
		.long idt_start

.section .data

	gdt_start:
		/* Null entry */
		.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		/* Kernel code entry */
		.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
		/* Kernel data entry */
		.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00
		/* User code entry */
		.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFA, 0xCF, 0x00
		/* User data entry */
		.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xF2, 0xCF, 0x00

	gdt_info:
		.word gdt_info - gdt_start - 1
		.long gdt_start

.section .bss, "aw", @nobits

	idt_start:
		.skip 4096

	pagetabs:
		/* 16 MiB for the kernel binary and initial data */
		.skip 16384

	pagedir:
		.skip 4096

	stack_bottom:
		.skip 16384
	stack_top:
