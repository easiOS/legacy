#ifndef H_KERNEL
#define H_KERNEL

#include <stdint.h>
#include <dtables.h>

struct cpu_desc
{
  char vendor[16];
  char brand[48];
  uint32_t features1;
  uint32_t features2;
} __attribute__((packed, aligned(4)));

struct lua_apps
{
	char name[32];
	void* address;
} __attribute__((packed, aligned(4)));

void kpanic(const char* msg, registers_t regs);
void reboot(const char* reason);
struct cpu_desc* get_cpu_desc(void);

#endif
