		.global interrupt_disable
		.global interrupt_enable

		.global isr_0
		.global isr_1
		.global isr_2
		.global isr_3
		.global isr_4
		.global isr_5
		.global isr_6
		.global isr_7
		.global isr_8
		.global isr_9
		.global isr_10
		.global isr_11
		.global isr_12
		.global isr_13
		.global isr_14
		.global isr_16
		.global isr_17
		.global isr_18
		.global isr_19
		.global isr_20
		.global isr_30

		.global irq_0
		.global irq_1
		.global irq_2
		.global irq_3
		.global irq_4
		.global irq_5
		.global irq_6
		.global irq_7
		.global irq_8
		.global irq_9
		.global irq_10
		.global irq_11
		.global irq_12
		.global irq_13
		.global irq_14
		.global irq_15

		.macro prologue

			pushl %eax
			pushl %ecx
			pushl %edx
			pushl %ebx
			pushl %ebp
			pushl %esi
			pushl %edi
			pushl %ds
			pushl %es
			pushl %fs
			pushl %gs

			movl $0x10, %ebp
			movl %ebp, %ds
			movl %ebp, %es
			movl %ebp, %fs
			movl %ebp, %gs

			movl %cr2, %ebp
			pushl %ebp

		.endm

		.macro epilogue bytes

			popl %ebp
			movl %ebp, %cr2

			popl %gs
			popl %fs
			popl %es
			popl %ds
			popl %edi
			popl %esi
			popl %ebp
			popl %ebx
			popl %edx
			popl %ecx
			popl %eax

			addl $\bytes, %esp
			iret

		.endm

		.macro isr_common num

			pushl $\num
			pushl $isr_XX_str
			call log_printf
			addl $8, %esp
			jmp halt

		.endm

		.macro irq_common num

			pushl $\num
			pushl $irq_XX_str
			call log_printf
			addl $8, %esp
			pushl $\num
			call pic_send_eoi
			addl $4, %esp

		.endm

.section .text

	isr_0:
		prologue
		movl 48(%esp), %eax
		pushl %eax
		pushl $isr_0_str
		call panic

	isr_1:
		prologue
		pushl $isr_1_str
		call panic

	isr_2:
		prologue
		pushl $isr_2_str
		call panic

	isr_3:
		prologue
		pushl $isr_3_str
		call panic

	isr_4:
		prologue
		movl 48(%esp), %eax
		pushl %eax
		pushl $isr_4_str
		call panic

	isr_5:
		prologue
		movl 48(%esp), %eax
		pushl %eax
		pushl $isr_5_str
		call panic

	isr_6:
		prologue
		movl 48(%esp), %eax
		pushl %eax
		pushl $isr_6_str
		call panic

	isr_7:
		prologue
		movl 48(%esp), %eax
		pushl %eax
		pushl $isr_7_str
		call panic

	isr_8:
		prologue
		pushl $isr_8_str
		call panic

	isr_9:
		prologue
		pushl $isr_9_str
		call panic

	isr_10:
		prologue
		movl 48(%esp), %ebx
		movl 52(%esp), %eax
		pushl %ebx
		pushl %eax
		pushl $isr_10_str
		call panic

	isr_11:
		prologue
		movl 48(%esp), %ebx
		movl 52(%esp), %eax
		pushl %ebx
		pushl %eax
		pushl $isr_11_str
		call panic

	isr_12:
		prologue
		movl 48(%esp), %ebx
		movl 52(%esp), %eax
		pushl %ebx
		pushl %eax
		pushl $isr_12_str
		call panic

	isr_13:
		prologue
		movl 48(%esp), %ebx
		movl 52(%esp), %eax
		pushl %ebx
		pushl %eax
		pushl $isr_13_str
		call panic

	isr_14:
		prologue
		movl 48(%esp), %ebx
		movl 52(%esp), %eax
		pushl %ebx
		pushl %eax
		pushl $isr_14_str
		call panic

	isr_16:
		prologue
		movl 48(%esp), %ebx
		movl 52(%esp), %eax
		pushl %ebx
		pushl %eax
		pushl $isr_16_str
		call panic

	isr_17:
		prologue
		movl 48(%esp), %ebx
		movl 52(%esp), %eax
		pushl %ebx
		pushl %eax
		pushl $isr_17_str
		call panic

	isr_18:
		prologue
		movl 48(%esp), %ebx
		movl 52(%esp), %eax
		pushl %ebx
		pushl %eax
		pushl $isr_18_str
		call panic

	isr_19:
		prologue
		movl 48(%esp), %ebx
		movl 52(%esp), %eax
		pushl %ebx
		pushl %eax
		pushl $isr_19_str
		call panic

	isr_20:
		prologue
		pushl $isr_20_str
		call panic

	isr_30:
		prologue
		pushl $isr_30_str
		call panic

	irq_0:
		prologue
		pushl $0
		call pic_send_eoi
		addl $4, %esp
		call pit_handle
		epilogue 0

	irq_1:
		prologue
		pushl $1
		call ps2_handle
.if 0 /* Disabled due to being reduntant. */
		addl $4, %esp
		pushl $1
.endif
		call pic_send_eoi
		addl $4, %esp
		epilogue 0

	irq_2:
		prologue
		irq_common 2
		epilogue 0

	irq_3:
		prologue
		irq_common 3
		epilogue 0

	irq_4:
		prologue
		irq_common 4
		epilogue 0

	irq_5:
		prologue
		irq_common 5
		epilogue 0

	irq_6:
		prologue
		irq_common 6
		epilogue 0

	irq_7:
		prologue
		irq_common 7
		epilogue 0

	irq_8:
		prologue
		irq_common 8
		epilogue 0

	irq_9:
		prologue
		irq_common 9
		epilogue 0

	irq_10:
		prologue
		irq_common 10
		epilogue 0

	irq_11:
		prologue
		irq_common 11
		epilogue 0

	irq_12:
		prologue
		pushl $2
		call ps2_handle
		addl $4, %esp
		pushl $12
		call pic_send_eoi
		addl $4, %esp
		epilogue 0

	irq_13:
		prologue
		irq_common 13
		epilogue 0

	irq_14:
		prologue
		irq_common 14
		epilogue 0

	irq_15:
		prologue
		irq_common 15
		epilogue 0

.section .rodata

	isr_0_str:
		.asciz "Panic: Divide By Zero Error: Instruction at address 0x%08X\n"

	isr_1_str:
		.asciz "Panic: Debug Exception\n"

	isr_2_str:
		.asciz "Panic: Non Maskable Interrupt\n"

	isr_3_str:
		.asciz "Panic: Breakpoint Exception\n"

	isr_4_str:
		.asciz "Panic: Overflow Exception: Instruction at address 0x%08X\n"

	isr_5_str:
		.asciz "Panic: Bound Range Exceeded: Instruction at address 0x%08X\n"

	isr_6_str:
		.asciz "Panic: Invalid Opcode Error: Instruction at address 0x%08X\n"

	isr_7_str:
		.asciz "Panic: Device Not Available: Instruction at address 0x%08X\n"

	isr_8_str:
		.asciz "Panic: Double Fault\n"

	isr_9_str:
		.asciz "Panic: Coprocessor Segment Overrun\n"

	isr_10_str:
		.ascii "Panic: Invalid Task State Segment: "
		.asciz "Instruction at address 0x%08X: Error code 0x%08X\n"

	isr_11_str:
		.ascii "Panic: Segment Not Present: Instruction at address 0x%08X: "
		.asciz "Error code: 0x%08X\n"

	isr_12_str:
		.ascii "Panic: Stack Segment Fault: Instruction at address 0x%08X: "
		.asciz "Error code: 0x%08X\n"

	isr_13_str:
		.ascii "Panic: General Protection Fault: "
		.asciz "Instruction at address 0x%08X: Error code 0x%08X\n"

	isr_14_str:
		.ascii "Panic: Page Fault: Instruction at address 0x%08X: "
		.asciz "Error code 0x%08X: Tried to access address 0x%08X\n"

	isr_16_str:
		.ascii "Panic: x87 Floating Point Exception: "
		.asciz "Instruction at address 0x%08X\n"

	isr_17_str:
		.ascii "Panic: Alignment Check Exception: "
		.asciz "Instruction at address 0x%08X\n"

	isr_18_str:
		.ascii "Panic: Machine Check Exception: "
		.asciz "Instruction at address 0x%08X\n"

	isr_19_str:
		.ascii "Panic: SIMD Floating Point Exception:"
		.asciz "Instruction at address 0x%08X\n"

	isr_20_str:
		.asciz "Panic: Virtualisation Exception\n"

	isr_30_str:
		.asciz "Panic: Security Exception\n"

	isr_XX_str:
		.asciz "Panic: ISR 0x%02hhX Occured\n"

	irq_XX_str:
		.asciz "IRQ 0x%02hhX Occured\n"
