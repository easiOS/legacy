all:
	if [ -a easios.bin ] ; \
		then \
		rm easios.bin ; \
	fi;
	nasm -felf32 boot.asm -o boot.o
	nasm -felf32 dtables.asm -o dtables2.o
	nasm -felf32 int.asm -o int.o
	i686-elf-gcc -c stdmem.c -o stdmem.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c timer.c -o timer.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c dtables.c -o dtables.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c video.c -o video.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c keyboard.c -o keyboard.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c itoa.c -o itoa.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c mouse.c -o mouse.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	#i686-elf-gcc -c realvideo.c -o realvideo.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c shell/shell_cmds.c -o shell_cmds.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c shell/shell.c -o shell.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c shell/shgfx.c -o shgfx.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra 
	i686-elf-gcc -T linker.ld -o easios.bin -ffreestanding -O2 -nostdlib boot.o int.o itoa.o stdmem.o \
		keyboard.o dtables2.o mouse.o dtables.o timer.o kernel.o video.o shgfx.o shell_cmds.o shell.o -lgcc

clean:
	rm easios.bin
	rm *.o
