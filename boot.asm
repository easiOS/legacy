;;Multiboot2 Header
;Multiboot2 constants
ARCH         equ  0 ;x86 architecture
MAGIC        equ  0xE85250D6 ;magic number

multiboot:
dd MAGIC
dd ARCH
dd multiboot-multiboot_end
dd -(MAGIC + ARCH + (multiboot-multiboot_end))

;framebuffer tag
;align 8
;dw 5,1
;dd 20
;dd 1024
;dd 768
;dd 24

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

; Currently the stack pointer register (esp) points at anything and using it may
; cause massive harm. Instead, we'll provide our own stack. We will allocate
; room for a small temporary stack by creating a symbol at the bottom of it,
; then allocating 16384 bytes for it, and finally creating a symbol at the top.
section .bootstrap_stack
align 4
stack_bottom:
times 16384 db 0
stack_top:

; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
section .text
global _start
_start:
	; Welcome to kernel mode! We now have sufficient code for the bootloader to
	; load and run our operating system. It doesn't do anything interesting yet.
	; Perhaps we would like to call printf("Hello, World\n"). You should now
	; realize one of the profound truths about kernel mode: There is nothing
	; there unless you provide it yourself. There is no printf function. There
	; is no <stdio.h> header. If you want a function, you will have to code it
	; yourself. And that is one of the best things about kernel development:
	; you get to make the entire system yourself. You have absolute and complete
	; power over the machine, there are no security restrictions, no safe
	; guards, no debugging mechanisms, there is nothing but what you build.

	; By now, you are perhaps tired of assembly language. You realize some
	; things simply cannot be done in C, such as making the multiboot header in
	; the right section and setting up the stack. However, you would like to
	; write the operating system in a higher level language, such as C or C++.
	; To that end, the next task is preparing the processor for execution of
	; such code. C doesn't expect much at this point and we only need to set up
	; a stack. Note that the processor is not fully initialized yet and stuff
	; such as floating point instructions are not available yet.

	; To set up a stack, we simply set the esp register to point to the top of
	; our stack (as it grows downwards).
	mov esp, stack_top

	; We are now ready to actually execute C code. We cannot embed that in an
	; assembly file, so we'll create a kernel.c file in a moment. In that file,
	; we'll create a C entry point called kernel_main and call it here.
	extern kernel_main
	call kernel_main

	; In case the function returns, we'll want to put the computer into an
	; infinite loop. To do that, we use the clear interrupt ('cli') instruction
	; to disable interrupts, the halt instruction ('hlt') to stop the CPU until
	; the next interrupt arrives, and jumping to the halt instruction if it ever
	; continues execution, just to be safe.
	cli
.hang:
	hlt
	jmp .hang
