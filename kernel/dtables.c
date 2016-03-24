#include <stddef.h>
#include <kernel.h>
#include <port.h>
#include <dtables.h>
#include <stdio.h>
#include <string.h>


extern void gdt_flush(unsigned);
extern void idt_flush(unsigned);

static void init_gdt();
static void gdt_set_gate(int,unsigned,unsigned,unsigned char,unsigned char);
static void init_idt();
static void idt_set_gate(unsigned char,unsigned,unsigned short,unsigned char);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

void dtables_init()
{
   // Initialise the global descriptor table.
   init_gdt();
   init_idt();
}

isr_t interrupt_handlers[256];

void dtables_reg(unsigned char n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

static void init_gdt()
{
   puts("Initializing GDT...");
   gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
   gdt_ptr.base  = (unsigned)&gdt_entries;

   gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
   gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
   gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
   gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
   gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

   gdt_flush((unsigned)&gdt_ptr);
   puts("done.\n");
}

// Set the value of one GDT entry.
static void gdt_set_gate(int num, unsigned base, unsigned limit, unsigned char access, unsigned char gran)
{
   gdt_entries[num].base_low    = (base & 0xFFFF);
   gdt_entries[num].base_middle = (base >> 16) & 0xFF;
   gdt_entries[num].base_high   = (base >> 24) & 0xFF;

   gdt_entries[num].limit_low   = (limit & 0xFFFF);
   gdt_entries[num].granularity = (limit >> 16) & 0x0F;

   gdt_entries[num].granularity |= gran & 0xF0;
   gdt_entries[num].access      = access;
}

static void init_idt()
{
   puts("Initializing IDT...");
   idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
   idt_ptr.base  = (unsigned)&idt_entries;

   memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

   // Remap the irq table.
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);


   idt_set_gate( 0, (unsigned)isr0 , 0x08, 0x8E);
   idt_set_gate( 1, (unsigned)isr1 , 0x08, 0x8E);
   idt_set_gate( 2, (unsigned)isr2 , 0x08, 0x8E);
   idt_set_gate( 3, (unsigned)isr3 , 0x08, 0x8E);
   idt_set_gate( 4, (unsigned)isr4 , 0x08, 0x8E);
   idt_set_gate( 5, (unsigned)isr5 , 0x08, 0x8E);
   idt_set_gate( 6, (unsigned)isr6 , 0x08, 0x8E);
   idt_set_gate( 7, (unsigned)isr7 , 0x08, 0x8E);
   idt_set_gate( 8, (unsigned)isr8 , 0x08, 0x8E);
   idt_set_gate( 9, (unsigned)isr9 , 0x08, 0x8E);
   idt_set_gate( 10, (unsigned)isr10 , 0x08, 0x8E);
   idt_set_gate( 11, (unsigned)isr11 , 0x08, 0x8E);
   idt_set_gate( 12, (unsigned)isr12 , 0x08, 0x8E);
   idt_set_gate( 13, (unsigned)isr13 , 0x08, 0x8E);
   idt_set_gate( 14, (unsigned)isr14 , 0x08, 0x8E);
   idt_set_gate( 15, (unsigned)isr15 , 0x08, 0x8E);
   idt_set_gate( 16, (unsigned)isr16 , 0x08, 0x8E);
   idt_set_gate( 17, (unsigned)isr17 , 0x08, 0x8E);
   idt_set_gate( 18, (unsigned)isr18 , 0x08, 0x8E);
   idt_set_gate( 19, (unsigned)isr19 , 0x08, 0x8E);
   idt_set_gate( 20, (unsigned)isr20 , 0x08, 0x8E);
   idt_set_gate( 21, (unsigned)isr21 , 0x08, 0x8E);
   idt_set_gate( 22, (unsigned)isr22 , 0x08, 0x8E);
   idt_set_gate( 23, (unsigned)isr23 , 0x08, 0x8E);
   idt_set_gate( 24, (unsigned)isr24 , 0x08, 0x8E);
   idt_set_gate( 25, (unsigned)isr25 , 0x08, 0x8E);
   idt_set_gate( 26, (unsigned)isr26 , 0x08, 0x8E);
   idt_set_gate( 27, (unsigned)isr27 , 0x08, 0x8E);
   idt_set_gate( 28, (unsigned)isr28 , 0x08, 0x8E);
   idt_set_gate( 29, (unsigned)isr29 , 0x08, 0x8E);
   idt_set_gate( 30, (unsigned)isr30 , 0x08, 0x8E);
   idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
   idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
   idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
   idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
   idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
   idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
   idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
   idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
   idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
   idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
   idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
   idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
   idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
   idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
   idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
   idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
   idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
   idt_flush((unsigned)&idt_ptr);
   puts("done.\n");
}

static void idt_set_gate(unsigned char num, unsigned base, unsigned short sel, unsigned char flags)
{
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   idt_entries[num].flags   = flags /* | 0x60 */;
}

static const char* errorstr[] = {"Division by zero", "Debug exception",
  "Non-maskable interrupt", "Breakpoint exception", "Into detected overflow",
  "Out of bounds exception", "Invalid opcode exception", "No coprocessor exception",
  "Double fault", "Coprocessor segment overrun", "Bad TSS", "Segment not present",
  "Stack fault", "General protection fault", "Page fault", "Unknown interrupt exception",
  "Coprocessor fault", "Alignment check exception", "Machine check exception"};

void isr_handler(registers_t regs)
{
   if (interrupt_handlers[regs.int_no] != 0 && regs.int_no < 32)
    {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
    else
    {
      kpanic(errorstr[regs.int_no], regs);
    }
}

void irq_handler(registers_t regs)
{
  if (interrupt_handlers[regs.int_no] != 0)
   {
       isr_t handler = interrupt_handlers[regs.int_no];
       handler(regs);
   }
   else
   {
     printf("\n[!!!] Unhandled interrupt: %d\n", regs.int_no);
   }
   // Send an EOI (end of interrupt) signal to the PICs.
   // If this interrupt involved the slave.
   if (regs.int_no >= 40)
   {
       // Send reset signal to slave.
       outb(0xA0, 0x20);
   }
   // Send reset signal to master. (As well as slave, if necessary).
   outb(0x20, 0x20);
}
