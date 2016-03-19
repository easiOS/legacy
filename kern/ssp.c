#include <stdint.h>
#include <stdlib.h>
#include <dtables.h>
#include <kernel.h>

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xD091E0F1
#else
#define STACK_CHK_GUARD 0x5a6d4a3855628b46 
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn))
void __stack_chk_fail(void)
{
#if __STDC_HOSTED__
	abort();
#else
	registers_t regs;
	kpanic("Error: Stack Smashing detected", regs);
	hlt_loop:
  	cpu_relax();
  	goto hlt_loop;
#endif
}