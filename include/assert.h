#ifndef H_ASSERT
#define H_ASSERT

#include <kernel.h>

#define assert(e) if(!e){ \
	printf("assertion in kernel failed in %s on line %d\n", __FILE__, __LINE__); \
	registers_t regs; \
	kpanic("KERNEL_ASSERT", regs); \
	}

#endif /* H_ASSERT */