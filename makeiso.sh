#!/bin/bash

rm -f easios.iso;

if [ "$#" -lt 1 ]; then
	echo "Usage: $0 binary";
	echo "Where binary is the path to the easios.bin file";
	exit 1;
fi

if [ ! -d "iso/boot" ]; then
	echo "Error: Directory iso/boot/ doesn't exist";
	exit 2;
fi

if [ ! -d "iso/boot/grub" ]; then
	echo "Error: Directory iso/boot/grub/ doesn't exist";
	exit 4;
fi

cp -v "$1" iso/boot/

grub-mkrescue  -d /usr/lib/grub/i386-pc/ -o easios.iso iso
