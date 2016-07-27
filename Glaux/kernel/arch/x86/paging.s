		.global paging_flush_tlb

.section .text

	paging_flush_tlb:
		movl %cr3, %eax
		movl %eax, %cr3
		ret
