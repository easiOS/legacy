section .text

global _start
_start:
  jmp multiboot_entry

;;Multiboot2 Header

align 8

multiboot:
dd 0xE85250D6
dd 0
dd multiboot-multiboot_end
dq -(0xE85250D6 + (multiboot-multiboot_end))

entry_address_tag_start:
dw 3,1
dd entry_address_tag_end - entry_address_tag_start
dd multiboot_entry
entry_address_tag_end:

framebuffer_tag_start:
dw 5,1
dd framebuffer_tag_end - framebuffer_tag_start
dd 800
dd 600
dd 24
framebuffer_tag_end:

end_tag_start:
dw 0,0
dd end_tag_end - end_tag_start
end_tag_end:
multiboot_end:

extern kmain

multiboot_entry:
  mov esp, stack_top
  push 0
  popf
  push ebx
  push eax
  call kmain
  cli
  hlt

section .stack, nobits
align 4
stack_bottom:
  resb 0x4000
stack_top:
