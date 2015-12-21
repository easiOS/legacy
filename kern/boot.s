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
  mov esp, stack_top
  push 0
  popf
  push ebx
  push eax
  call kmain
  cli
  hlt

global topkek
topkek:
  push 1
  push text
  push text_end - text_start
  call 0x100
  add esp, 12
  ret

section .stack, nobits
align 4
stack_bottom:
  resb 0x4000
stack_top:

section .data
text_start:
text db "topkek", 13, 0
text_end:
