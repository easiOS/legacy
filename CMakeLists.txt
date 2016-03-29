cmake_minimum_required(VERSION 2.8.9)
project (easiOS)

include_directories(include)

ENABLE_LANGUAGE(ASM)

set(CMAKE_C_COMPILER i686-elf-gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -std=gnu99 -ffreestanding -Wall -fstack-protector-all")
set(CMAKE_ASM_COMPILE_OBJECT "nasm -felf32 -o <OBJECT> <SOURCE>")
set(CMAKE_EXE_LINKER_FLAGS "-T linker.ld -ffreestanding -O2 -nostdlib -lgcc")
set(CMAKE_C_LINK_EXECUTABLE "i686-elf-gcc -T ${CMAKE_SOURCE_DIR}/linker.ld -o <TARGET> -ffreestanding -O2 -nostdlib <OBJECTS>")
file(GLOB SOURCES_C "*/*.c")
file(GLOB SOURCES_A "kernel/*.s")

set(VERBOSE 1)

add_executable(easios.bin ${SOURCES_C} ${SOURCES_A})
