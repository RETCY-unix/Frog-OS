// Registers saved on interrupt
struct registers {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
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

// ISR handler (CPU exceptions) - just halt on exception
void isr_handler(struct registers* regs) {
    // Exception occurred - halt
    while (1) {
        __asm__ __volatile__("hlt");
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
    
    // Send EOI to PICs
    if (regs->int_no >= 40) {
        __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0xA0));
    }
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0x20));
}
