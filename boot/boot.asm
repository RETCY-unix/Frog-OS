BITS 16
ORG 0x7C00

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    mov [boot_drive], dl

    mov si, msg_loading
    call print_string

    ; Load MORE sectors - increase from 20 to 50
    mov ah, 0x02
    mov al, 50          ; Load 50 sectors (25KB)
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    mov bx, 0x1000
    int 0x13
    
    jc disk_error

    mov si, msg_success
    call print_string

    cli
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp 0x08:protected_mode

disk_error:
    mov si, msg_error
    call print_string
    jmp $

print_string:
    pusha
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .loop
.done:
    popa
    ret

boot_drive db 0
msg_loading db 'Loading kernel...', 13, 10, 0
msg_success db 'Jumping to kernel!', 13, 10, 0
msg_error db 'DISK ERROR!', 13, 10, 0

gdt_start:
    dd 0x0, 0x0
    dw 0xFFFF, 0x0
    db 0x0, 10011010b, 11001111b, 0x0
    dw 0xFFFF, 0x0
    db 0x0, 10010010b, 11001111b, 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

BITS 32
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    
    call 0x1000
    
    jmp $

times 510-($-$$) db 0
dw 0xAA55
