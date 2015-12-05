#ifndef H_KERNEL
#define H_KERNEL
#include <dtables.h>
void kpanic(const char* msg, registers_t regs);
void reboot();
void halt();
uint32_t* get_boot_date();
void switch_to_user_mode(); //NE HASZNÁLD (13)
#endif
