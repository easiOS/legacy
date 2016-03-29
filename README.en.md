# easiOS
easiOS is an in-development operating system for the x86 architecture.

## Features
* Cooperative software multitasking
* Keyboard-oriented window manager with design inspired by the ＡＥＳＴＨＥＴＩＣＳ of Windows 2000
* Lua 5.1 bytecode as executable
* Multiboot2 compliant
* FAT32 filesystem

## How to build
### Requirements:
* CMake https://cmake.org/
* Netwide Assembler http://www.nasm.us/
* GCC i686 Cross-Compiler http://wiki.osdev.org/GCC_Cross-Compiler
### Unix
1. Create a separate build-directory (e.g. ~/build)
2. `cd` into the build-directory
3. Run `cmake sourcedir`, where sourcedir is the directory where the source code is
4. Run `make`
### Windows
*WARNING: this is untested*
1. Create a separate build-directory (e.g. Documents\build)
2. Open CMake GUI
3. Set the source code and build directory paths
4. Press Configure, select your Visual Studio version as generator
5. Press Generate
6. Open VS Solution
7. Build VS Solution

## How to run
### Requirements:
* Multiboot2 compatible bootloader (syslinux, grub2, etc.)
* IBM PC: CPU with SSE or better (Pentium 3 or better recommended), 256MB memory or more

### In a virtual Machine
1. Build easiOS like described above
2. a) On Unix systems, run makeiso.sh
2. b) On Windows systems, run makeiso.bat
2. Mount the ISO in the VM
3. Run the VM

### On a real machine
1. Build easiOS like described above
2. Copy easios.bin from root directory of the source to your boot directory
3. Add an entry for easiOS in your bootloader's config files (sample for GRUB2 included in /iso/boot/grub/grub.cfg)

## What works
## Supported devices

### Functioning

### Non-functioning