#!/usr/bin/python3

import subprocess
import sys
import os
import copy
import shutil

EXECUTABLE="build/easios.bin"
ISO="easios.iso"
PO2MOC="msgfmt"
PO2MOCFLAGS="-o $OUTPUT $INPUT"

EXT = {
    "po" : {
        "C" : PO2MOC,
        "F" : PO2MOCFLAGS
    }
}

def iso(b):
    print("building ISO...");
    if os.path.isfile(ISO):
        os.remove(ISO)
    if not os.path.isdir("iso"):
        os.mkdir("iso")
    if not os.path.isdir("iso/boot"):
        os.mkdir("iso/boot")
    if not os.path.isfile(b):
        print("easiOS kernel file not found. please run buildiso as \"" + sys.argv[0] + " iso [path to kernel file]\"")
        return
    shutil.copy(b, "iso/boot/")
    try:
        subprocess.run(args=["grub-mkrescue", "-d", "/usr/lib/grub/i386-pc/", "-o", ISO, "iso"])
    except:
        print("grub-mkrescure not installed, skipping");

def transl():
    print("building translation files...");
    if os.path.isfile("iso/boot/grub/locale/eos.po"):
        try:
            subprocess.run(args=[PO2MOC]+PO2MOCFLAGS.replace("$INPUT", "iso/boot/grub/locale/eos.po").replace("$OUTPUT", "iso/boot/grub/locale/eos.mo").split())
        except FileNotFoundError:
            print("msgfmt not installed, skipping");

if __name__ == "__main__":
    print("easiOS build system\n===============");
    if len(sys.argv) > 1:
        if "iso" in sys.argv:
            if len(sys.argv) > 2:
                iso(sys.argv[2])
            else:
                iso("build/easios.bin")
        if "transl" in sys.argv:
            transl()
    else:
        transl()
        iso("build/easios.bin")
