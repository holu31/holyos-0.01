.equ PAGE_PRESENT, 1 << 0
.equ PAGE_WRITE,   1 << 1
.equ PAGE_USER,    1 << 2

.section .text
.global load_page_directory, enable_paging, flush_tlb

load_page_directory:
	movl 4(%esp), %eax
	movl %eax, %cr3
	ret

enable_paging:
	movl %cr0, %eax
	orl $0x80000000, %eax
	movl %eax, %cr0
	ret

flush_tlb:
	movl 4(%esp), %eax
	invlpg (%eax)
	ret
