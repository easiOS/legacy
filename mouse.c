#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include "port.h"
#include "dtables.h"
#include "video.h"

bool wasclick = false;

static void mouse_callback(registers_t regs)
{
  wasclick = true;
}

bool was_click()
{
  return wasclick;
}

void init_mouse()
{
    terminal_prfxi(ticks(), "Mouse\n");
    register_interrupt_handler(IRQ12, &mouse_callback);
}