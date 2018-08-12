
align 4
global irq1handler
irq1handler:
	cld
	mov al, 0x20
	mov al, 0x20
	hlt
	ret
