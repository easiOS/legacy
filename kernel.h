#ifndef H_KERNEL
#define H_KERNEL

#define KERNEL_NAME "AvenueOS\n"
#define KERNEL_VERSION "0.2.3"

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include "multiboot2.h"
#include "stdmem.h"
#include "video.h"
#include "dtables.h"
#include "timer.h"
#include "keyboard.h"
#include "timer.h"
#include "mouse.h"
//#include "realvideo.h"
#include "shell/shell.h"

void kpanic();
void reboot();
void halt();
uint32_t get_boot_date();
void switch_to_user_mode(); //NE HASZNÁLD (13)
void oscall_handler(registers_t regs);
#endif