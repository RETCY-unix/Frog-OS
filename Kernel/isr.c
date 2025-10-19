#include "../Lib/include/screen.h"

// Registers saved on interrupt
struct registers {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

// Exception messages
static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// IRQ handler function pointers
typedef void (*irq_handler_t)(struct registers*);
static irq_handler_t irq_handlers[16] = {0};

// Register an IRQ handler
void irq_install_handler(int irq, irq_handler_t handler) {
    irq_handlers[irq] = handler;
}

// Uninstall an IRQ handler
void irq_uninstall_handler(int irq) {
    irq_handlers[irq] = 0;
}

// ISR handler (CPU exceptions)
void isr_handler(struct registers* regs) {
    if (regs->int_no < 32) {
        screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
        screen_print("EXCEPTION: ");
        screen_println(exception_messages[regs->int_no]);
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        
        // Halt on exception
        while (1) {
            __asm__ __volatile__("hlt");
        }
    }
}

// IRQ handler (hardware interrupts)
void irq_handler(struct registers* regs) {
    // Call registered handler if exists
    if (regs->int_no >= 32 && regs->int_no <= 47) {
        int irq = regs->int_no - 32;
        if (irq_handlers[irq] != 0) {
            irq_handlers[irq](regs);
        }
    }
    
    // Send EOI (End of Interrupt) to PICs
    if (regs->int_no >= 40) {
        // Send EOI to slave PIC
        __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0xA0));
    }
    // Send EOI to master PIC
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0x20));
}
