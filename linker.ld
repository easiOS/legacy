ENTRY(_start)

SECTIONS
{
	. = 1M;

	.text BLOCK(32M) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(32M) : ALIGN(4K)
	{
		*(.rodata)
	}


	.data BLOCK(32M) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(32M) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}
}
