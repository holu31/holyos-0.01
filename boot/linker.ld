ENTRY(_start)

SECTIONS
{
	. = 2M;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
	
	.ctors : {
		 __ctors_start = .;
		KEEP(*(.ctors))
		KEEP(*(.init_array))
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
		__ctors_end = .;
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		endkernel = .;
	}
}

