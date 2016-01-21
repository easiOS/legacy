section .text

global _start
_start:
  jmp multiboot_entry

;;Multiboot2 Header

align 8

multiboot:
dd 0xE85250D6
dd 0
dd multiboot_end-multiboot
dd 0xFFFFFFFF & -(0xE85250D6 + (multiboot_end-multiboot))

align 8 ; KÉT TEG KÖZÉ EZ KELL IDE

entry_address_tag_start:
dw 3,1
dd entry_address_tag_end - entry_address_tag_start
dd multiboot_entry
entry_address_tag_end:

align 8 ; KÉT TEG KÖZÉ EZ KELL IDE

framebuffer_tag_start:
dw 5,1
dd framebuffer_tag_end - framebuffer_tag_start
dd 1024
dd 768
dd 32
framebuffer_tag_end:

align 8 ; KÉT TEG KÖZÉ EZ KELL IDE

end_tag_start:
dw 0,0
dd end_tag_end - end_tag_start
end_tag_end:
multiboot_end:

extern kmain

multiboot_entry:
  mov ecx, cr0
  and ecx, 0x7FFFFFFF
  mov cr0, ecx
  mov esp, stack_top
  push 0
  popf
  push ebx
  push eax
  call kmain
  cli
  hlt

global sse_enable

sse_enable:
  push ebp
  mov ebp, esp
  mov eax, cr0
  and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
  or ax, 0x2			;set coprocessor monitoring  CR0.MP
  mov cr0, eax
  mov eax, cr4
  or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
  mov cr4, eax
  pop ebp
  ret

section .stack, nobits
align 4
stack_bottom:
  resb 0x4000
stack_top:
