#include "../Lib/include/idt.h"
#include "../Lib/include/screen.h"

// IDT with 256 entries
struct idt_entry idt[256];
struct idt_ptr idtp;

// Remap PIC (Programmable Interrupt Controller)
static void pic_remap() {
    // ICW1 - Start initialization
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x11), "Nd"((unsigned short)0x20));
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x11), "Nd"((unsigned short)0xA0));
    
    // ICW2 - Remap IRQ 0-7 to 32-39, IRQ 8-15 to 40-47
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0x21));
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x28), "Nd"((unsigned short)0xA1));
    
    // ICW3 - Setup cascade
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x04), "Nd"((unsigned short)0x21));
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x02), "Nd"((unsigned short)0xA1));
    
    // ICW4 - 8086 mode
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x01), "Nd"((unsigned short)0x21));
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0x01), "Nd"((unsigned short)0xA1));
    
    // Mask all interrupts except keyboard (IRQ1) and mouse (IRQ12)
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0xFD), "Nd"((unsigned short)0x21));
    __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0xEF), "Nd"((unsigned short)0xA1));
}

// Set an IDT gate
void idt_set_gate(unsigned char num, unsigned int base, unsigned short selector, unsigned char flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// Initialize IDT
void idt_init() {
    int i;
    
    // Set up IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;
    
    // Clear IDT
    for (i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Remap PIC
    pic_remap();
    
    // Install ISRs (CPU exceptions)
    idt_set_gate(0, (unsigned int)isr_0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned int)isr_1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned int)isr_2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned int)isr_3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned int)isr_4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned int)isr_5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned int)isr_6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned int)isr_7, 0x08, 0x8E);
    idt_set_gate(8, (unsigned int)isr_8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned int)isr_9, 0x08, 0x8E);
    idt_set_gate(10, (unsigned int)isr_10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned int)isr_11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned int)isr_12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned int)isr_13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned int)isr_14, 0x08, 0x8E);
    idt_set_gate(15, (unsigned int)isr_15, 0x08, 0x8E);
    idt_set_gate(16, (unsigned int)isr_16, 0x08, 0x8E);
    idt_set_gate(17, (unsigned int)isr_17, 0x08, 0x8E);
    idt_set_gate(18, (unsigned int)isr_18, 0x08, 0x8E);
    idt_set_gate(19, (unsigned int)isr_19, 0x08, 0x8E);
    idt_set_gate(20, (unsigned int)isr_20, 0x08, 0x8E);
    idt_set_gate(21, (unsigned int)isr_21, 0x08, 0x8E);
    idt_set_gate(22, (unsigned int)isr_22, 0x08, 0x8E);
    idt_set_gate(23, (unsigned int)isr_23, 0x08, 0x8E);
    idt_set_gate(24, (unsigned int)isr_24, 0x08, 0x8E);
    idt_set_gate(25, (unsigned int)isr_25, 0x08, 0x8E);
    idt_set_gate(26, (unsigned int)isr_26, 0x08, 0x8E);
    idt_set_gate(27, (unsigned int)isr_27, 0x08, 0x8E);
    idt_set_gate(28, (unsigned int)isr_28, 0x08, 0x8E);
    idt_set_gate(29, (unsigned int)isr_29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned int)isr_30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned int)isr_31, 0x08, 0x8E);
    
    // Install IRQs (hardware interrupts) - mapped to 32-47
    idt_set_gate(32, (unsigned int)irq_0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned int)irq_1, 0x08, 0x8E);  // Keyboard
    idt_set_gate(34, (unsigned int)irq_2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned int)irq_3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned int)irq_4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned int)irq_5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned int)irq_6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned int)irq_7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned int)irq_8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned int)irq_9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned int)irq_10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned int)irq_11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned int)irq_12, 0x08, 0x8E); // Mouse
    idt_set_gate(45, (unsigned int)irq_13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned int)irq_14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned int)irq_15, 0x08, 0x8E);
    
    // Load IDT
    idt_load((unsigned int)&idtp);
    
    // Enable interrupts
    __asm__ __volatile__("sti");
}
