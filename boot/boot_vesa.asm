BITS 16
ORG 0x7C00

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    mov [drive], dl

    ; Load kernel (50 sectors)
    mov ah, 0x02
    mov al, 50
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [drive]
    mov bx, 0x1000
    int 0x13
    jc hang

    ; Set VESA mode 0x118 (1024x768x24)
    mov ax, 0x4F02
    mov bx, 0x4118
    int 0x10

    ; Store framebuffer at 0x7E00
    mov dword [0x7E00], 0xFD000000
    mov word [0x7E04], 1024
    mov word [0x7E06], 768
    mov word [0x7E08], 3072
    mov byte [0x7E0A], 24

    cli
    lgdt [gdt_desc]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:pm_mode

hang:
    jmp $

drive db 0

gdt_start:
    dq 0
    dw 0xFFFF, 0
    db 0, 10011010b, 11001111b, 0
    dw 0xFFFF, 0
    db 0, 10010010b, 11001111b, 0
gdt_end:

gdt_desc:
    dw gdt_end - gdt_start - 1
    dd gdt_start

BITS 32
pm_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000
    call 0x1000
    jmp $

times 510-($-$$) db 0
dw 0xAA55
