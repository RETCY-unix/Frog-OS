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

    ; Try to set VESA mode 0x11B (1280x1024x24) first
    ; Or 0x14D (1920x1080x24) if supported
    ; Fall back to 0x118 (1024x768x24) if not available
    
    ; Get VESA info
    mov ax, 0x4F00
    mov di, 0x7000
    int 0x10
    cmp ax, 0x004F
    jne use_fallback_mode
    
    ; Try 1920x1080x32 (mode 0x14D or custom)
    mov ax, 0x4F01
    mov cx, 0x14D
    mov di, 0x7100
    int 0x10
    cmp ax, 0x004F
    je set_1920x1080
    
    ; Try 1280x1024x24 (mode 0x11B)
    mov ax, 0x4F01
    mov cx, 0x11B
    mov di, 0x7100
    int 0x10
    cmp ax, 0x004F
    je set_1280x1024
    
use_fallback_mode:
    ; Fall back to 1024x768x24 (mode 0x118)
    mov ax, 0x4F02
    mov bx, 0x4118
    int 0x10
    
    ; Store framebuffer info at 0x7E00 for 1024x768
    mov dword [0x7E00], 0xFD000000
    mov word [0x7E04], 1024
    mov word [0x7E06], 768
    mov word [0x7E08], 3072
    mov byte [0x7E0A], 24
    jmp enter_pmode

set_1920x1080:
    ; Set mode 1920x1080x32
    mov ax, 0x4F02
    mov bx, 0x414D  ; Try with LFB bit set
    int 0x10
    
    ; Get mode info
    mov ax, 0x4F01
    mov cx, 0x14D
    mov di, 0x7100
    int 0x10
    
    ; Store framebuffer info
    mov eax, [0x7128]  ; Physical base pointer
    mov [0x7E00], eax
    mov word [0x7E04], 1920
    mov word [0x7E06], 1080
    mov word [0x7E08], 7680  ; pitch: 1920 * 4
    mov byte [0x7E0A], 32
    jmp enter_pmode

set_1280x1024:
    ; Set mode 1280x1024x24
    mov ax, 0x4F02
    mov bx, 0x411B
    int 0x10
    
    ; Store framebuffer info
    mov dword [0x7E00], 0xFD000000
    mov word [0x7E04], 1280
    mov word [0x7E06], 1024
    mov word [0x7E08], 3840  ; pitch: 1280 * 3
    mov byte [0x7E0A], 24
    jmp enter_pmode

enter_pmode:
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
