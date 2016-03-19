# easiOS
easiOS is an in-development operating system for the x86 architecture.

## Features
* Cooperative software multitasking
* Keyboard-oriented window manager with design inspired by the ＡＥＳＴＨＥＴＩＣＳ of Windows 2000
* Lua 5.1 bytecode as executable
* Multiboot2 compliant
* FAT32 filesystem

## How to build
*build system is being rewritten*

## How to run
### Requirements:
* Multiboot2 compatible bootloader (syslinux, grub2, etc.)
* IBM PC: CPU with SSE or better (Pentium 3 or better recommended), 256MB memory or more

### In a virtual Machine
1. Build easiOS like described above
2. Mount the ISO (found in the root directory of the source) in the VM
3. Run the VM

### On a real machine
1. Build easiOS like described above
2. Copy easios.bin from root directory of the source to your boot directory
3. Add an entry for easiOS in your bootloader's config files (sample for GRUB2 included in /iso/boot/grub/grub.cfg)

## What works
## Supported devices

### Functioning

### Non-functioning