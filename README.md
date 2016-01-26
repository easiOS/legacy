*„Lasciate ogni speranza, voi ch'entrate.”*

# easiOS
easiOS is an in-development <span title="lol :-D :-D :DDD">operating system</span> for x86.

## Features
* Multi-user
* Cooperative software multitasking
* Keyboard oriented window manager with design inspired by the ﻿ＡＥＳＴＨＥＴＩＣＳ of Windows 2000
* Lua 5.1 bytecode as executable
* Multiboot2 compliant

## How to build
### Requirements:
* GCC cross-compiler for i686 ELF (i686-elf-gcc)
* Netwide Assembler (nasm)
* Python 3
* GRUB2 (optional, for building ISO)
1. Download source code (git clone, tarball, etc.)
2. Run ./build

## How to run
### Requirements:
* Multiboot2 compatible bootloader (syslinux, grub2, etc.)
* x86 PC: CPU with SSE or better (Pentium 3 or better recommended), 256MB memory or better

### In a virtual Machine
1. Build easiOS like described above
2. Mount the ISO (found in the root directory of the source) in the VM
3. Run the VM

### On a real machine
1. Build easiOS like described above
2. Copy easios.bin from root directory of the source to your boot directory
3. Add an entry for easiOS in your bootloader's config files (sample for GRUB2 included in /iso/boot/grub/grub.cfg)

## What works
* Single-user mode
* Cooperative software multitasking
* Window manager (needs optimization)
* PS/2 Keyboard & Mouse
* Serial port communication
* PIT
* Memory management
* Graphical mode and drawing

## Supported devices

### Functioning
* PS/2 Keyboard
* Serial ports
* PIT
* Virtualbox Graphics Adapter (setting mode works)

### Non-functioning
* PS/2 Mouse (actually, works but glitchy af)
* ATI RV100 gfx card (PCI)
* NE2000 ethernet card (PCI)
* PCNET32 ethernet card (PCI)
* VirtIO Network Device (PCI)
* USB Enhanced Host Controller Interface (PCI)

## How multitasking works
Every window has (as of v0.3.3) 5 callbacks: load, unload, update, event, draw. Load is called after the window is 
spawned, unload is called before the window is destroyed. The window manager calls event and update for every window in 
the main loop. The event callback handles keyboard and mouse events. In update, the program does things like calculations. 
In draw, the window should not do anything other than drawing, as draw is only called for the current active window.
