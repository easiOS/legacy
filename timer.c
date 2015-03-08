#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include "port.h"
#include "dtables.h"
#include "timer.h"
#include "video.h"

uint32_t tick = 0;

static void timer_callback(registers_t regs)
{
    tick++;
}

uint32_t ticks()
{
    return tick;
}

uint32_t millis()
{
    return tick;
}

void init_timer(uint32_t frequency)
{
    terminal_prfxi(ticks(), "Timer register, frequency: ");
    terminal_writeint(frequency);
    terminal_writestring("Hz...");
    // Firstly, register our timer callback.
    register_interrupt_handler(IRQ0, &timer_callback);

    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency. Important to note is
    // that the divisor must be small enough to fit into 16-bits.
    uint32_t divisor = 1193180 / frequency;
    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
    terminal_writestring("done.\n");
}

void sleep(uint32_t millis)
{
    uint32_t start = ticks();
    uint32_t now = ticks();
    while(now - start < millis)
    {
        now = ticks();
        terminal_writestring("");
    }
}