#!/usr/bin/python3

import subprocess
import sys
import os
import copy
import shutil

CC="i686-elf-gcc"
AS="nasm"
LINKER="i686-elf-gcc"
CFLAGS="-O2 -c $INPUT -o $OUTPUT -std=gnu99 -ffreestanding -Wall \
-fdiagnostics-color=always -I include/ -fstack-protector-all"
AFLAGS="-felf32 -o $OUTPUT $INPUT"
EXECUTABLE="easios.bin"
ISO="easios.iso"
LDFLAGS="-T linker.ld -o $OUTPUT -ffreestanding -O2 -nostdlib $INPUT -lgcc"
PO2MOC="msgfmt"
PO2MOCFLAGS="-o $OUTPUT $INPUT"

EXT = {
    "c" : {
        "C" : CC,
        "F" : CFLAGS
    },
    "s" : {
        "C" : AS,
        "F" : AFLAGS
    },
    "po" : {
        "C" : PO2MOC,
        "F" : PO2MOCFLAGS
    }
}

SOURCES=["kernel/boot.s",
         "kernel/kernel.c", "kernel/video.c", "kernel/text.c", #"kernel/dtables.c",
         "kernel/memory.c", #"kernel/krandom.c", "kernel/users.c", "kernel/dma.c",
         "kernel/ssp.c", #"kernel/kshell.c",
         #"kernel/"
         "drivers/serial.c",
         ]

#SOURCES=["kernel/boot.s", "kernel/kernel.c", "kernel/text.c", "dev/serial.c",
#         "kernel/video.c", "stdlibc/stdio.c", "stdlibc/string.c",
#         "stdlibc/math.c",
#         "kernel/dtables.c", "kernel/dtablesa.s", "kern/int.s", "dev/timer.c",
#         "kern/libc/stdlib.c", "dev/kbd.c", "kern/mem.c", "dev/mouse.c",
#         "kern/libc/time.c", "kern/krandom.c", "dev/pci.c", "kern/md5.c",
#         "kern/users.c", "kern/vfs.c", "kern/acpi/rsdp.c",
#         "kern/dma.c", "dev/pci/virtboxgfx.c", "kern/ssp.c",
#         "kshell/kshell.c", "kern/kernupd.c",
#         "dev/pci/ne2k_pci.c", "dev/ethernet.c", "dev/pci/pcnet3.c",
#         "dev/pci/ohci.c", "dev/graphics.c",
#         "dev/pci/virtio-net.c", "dev/pci/debugdrv.c", "dev/pci/amd_rv100.c",
#         "dev/disk.c", "dev/pci/e100.c", "dev/pci/e1000.c",
#         "eelphant/eelphant.c", "eelphant/terminal.c", "eelphant/msgbox.c",
#         "eelphant/eclock.c", "eelphant/notepad.c", "eelphant/iowindow.c",
#         "eelphant/physdemo.c", "eelphant/luavm.c", "eelphant/efm.c",
#         "eelphant/testapp.c", "eelphant/login.c", "eelphant/ifconfig.c",
#         "net/icmp.c", "net/ipv4.c", "net/routing.c", "net/slip.c",
##         "net/udp.c", "net/arp.c",
#         "kern/fs/fat32.c",
#         "iso/boot/grub/locale/eos.po",
#         ]

NOWARN = ["kern/fs/fat32.c"]

def build():
    print("=== \033[95mStarting build...\033[0m ===")
    OBJECTS = []
    for f in SOURCES:
        print("Compiling " + f)
        if not os.path.isfile(f):
            print("File " + f + " doesn't exists, exiting.")
            sys.exit(0)
        fname = f.split(".")
        tmpfname = copy.copy(fname)
        tmpfname[len(tmpfname)-1] = "o"
        fout = ".".join(tmpfname)

        fext = fname[len(fname)-1]
        if not fext in ["c", "s"]: continue
        if not fext in EXT:
            print("Unknown file extension " + fext + ", exiting.")
            sys.exit(0)
        cc = EXT[fext]["C"]
        flags = EXT[fext]["F"]
        if "$INPUT" in flags:
            flags = flags.replace("$INPUT", f)
        else:
            flags += " " + f
        if "$OUTPUT" in flags:
            flags = flags.replace("$OUTPUT", fout)
        else:
            flags += " " + fout
        if f in NOWARN:
            flags += " -w"
        subprocess.run(args=[cc]+flags.split(), check=True)
        OBJECTS.append(fout)
    print("=== \033[95mLinking...\033[0m ===")
    flags = LDFLAGS
    if "$INPUT" in flags:
        flags = flags.replace("$INPUT", " ".join(OBJECTS))
    else:
        flags += " " + " ".join(OBJECTS)
    if "$OUTPUT" in flags:
        flags = flags.replace("$OUTPUT", EXECUTABLE)
    else:
        flags += " " + EXECUTABLE
    subprocess.run(args=[CC]+flags.split(), check=True)
    print("Output: " + os.path.abspath(EXECUTABLE))

def clean():
    print("Cleaning...")
    for f in SOURCES:
        fname = f.split(".")
        fname[len(fname)-1] = "o"
        nfn = ".".join(fname)
        if os.path.isfile(nfn):
            os.remove(nfn)
    if os.path.isfile(EXECUTABLE):
        os.remove(EXECUTABLE)
    if os.path.isfile(ISO):
        os.remove(ISO)

def iso():
    if os.path.isfile(ISO):
        os.remove(ISO)
    if not os.path.isdir("iso"):
        os.mkdir("iso")
    if not os.path.isdir("iso/boot"):
        os.mkdir("iso/boot")
    shutil.copy(EXECUTABLE, "iso/boot/")
    subprocess.run(args=["grub-mkrescue", "-d", "/usr/lib/grub/i386-pc/", "-o", ISO, "iso"])

def run():
    if os.path.isfile(ISO):
        subprocess.run(args=["qemu-system-i386", "-serial", "stdio", "-cdrom", ISO])

def transl():
    if os.path.isfile("iso/boot/grub/locale/eos.po"):
        subprocess.run(args=[PO2MOC]+PO2MOCFLAGS.replace("$INPUT", "iso/boot/grub/locale/eos.po").replace("$OUTPUT", "iso/boot/grub/locale/eos.mo").split())

if __name__ == "__main__":
    if len(sys.argv) > 1:
        if "clean" in sys.argv:
            clean()
        if "build" in sys.argv:
            build()
        if "iso" in sys.argv:
            iso()
        if "run" in sys.argv:
            run()
        if "transl" in sys.argv:
            transl()
    else:
        build()
        transl()
        iso()
