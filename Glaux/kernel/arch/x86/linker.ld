ENTRY (_start)

SECTIONS
{
	. = 0xE0100000;
	_kernel_start = .;
	_text_start = .;
	.text ALIGN (4K) : AT (ADDR (.text) - 0xE0000000)
	{
		*(.mboot)
		*(.text)
	}
	_text_end = .;
	_data_start = .;
	.data ALIGN (4K) : AT (ADDR (.data) - 0xE0000000)
	{
		*(.data)
	}
	_data_end = .;
	_rodata_start = .;
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xE0000000)
	{
		*(.rodata)
	}
	_rodata_end = .;
	_bss_start = .;
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xE0000000)
	{
		*(.bss)
	}
	_bss_end = .;
	_kernel_end = .;
}
