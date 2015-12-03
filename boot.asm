section .text
global start,_start
start:
_start:
	jmp multiboot_entry

align 8

;;Multiboot2 Header
multiboot:
dd 0xE85250D6
dd 0
dd multiboot-multiboot_end
dq -(0xE85250D6 + (multiboot-multiboot_end))

entry_address_tag_start:
dw 3,1
dd entry_address_tag_end - entry_address_tag_start
dd _start
entry_address_tag_end:

framebuffer_tag_start:
dw 5,1
dd framebuffer_tag_end - framebuffer_tag_start
dd 800
dd 600
dd 24
framebuffer_tag_end:

;end tag
align 8
dw 0,0
dd 8
multiboot_end:

; Multiboot1 constants
;MBALIGN     equ  1<<0                   ; align loaded modules on page boundaries
;MEMINFO     equ  1<<1                   ; provide memory map
;VBEINFO     equ  1<<2 ;provide video info
;FLAGS       equ  MBALIGN | MEMINFO | VBEINFO; this is the Multiboot 'flag' field
;MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
;CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot

;;old multiboot1 header
;align 4
;	dd MAGIC
;	dd FLAGS
;	dd CHECKSUM

; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
extern kernel_main
multiboot_entry:
	mov esp, stack_top
	;reset eflags
	push 0
	popf
	push ebx
	push eax
	call kernel_main
	cli

.hang:
	hlt
	jmp .hang

section .bootstrap_stack, nobits
	align 4
	stack_bottom:
	resb 16384
	resb 16384
	resb 16384
	resb 16384
stack_top:
