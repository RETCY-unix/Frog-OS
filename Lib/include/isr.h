#ifndef ISR_H
#define ISR_H

// Registers structure
struct registers {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

// Function prototypes
void isr_handler(struct registers* regs);
void irq_handler(struct registers* regs);
void irq_install_handler(int irq, void (*handler)(struct registers*));
void irq_uninstall_handler(int irq);

#endif
