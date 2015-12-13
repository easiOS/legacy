#ifndef H_KERNEL
#define H_KERNEL

#include <stdint.h>
#include <dtables.h>

void kpanic(const char* msg, registers_t regs);
void reboot(const char* reason);

#endif
