[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Affiche le message de boot
    mov si, msg
    call print_string

    ; Charger la GDT
    lgdt [gdt_descriptor]

    ; Activer le bit PE dans CR0
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Sauter vers le mode protégé
    jmp CODE_SEG:init_pm

print_string:
    .next:
        lodsb
        cmp al, 0
        je .done
        mov ah, 0x0E
        int 0x10
        jmp .next
    .done:
        ret

msg: db "EclipseOS is booting...", 0

; ---------------------
; Code mode protégé
; ---------------------
[BITS 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000     ; Stack

    ; Saut vers le kernel
    jmp 0x1000           ; Doit correspondre à l’adresse du kernel compilé

; ---------------------
; Définition GDT
; ---------------------
gdt_start:
    dq 0x0000000000000000     ; Null descriptor
    dq 0x00CF9A000000FFFF     ; Code segment
    dq 0x00CF92000000FFFF     ; Data segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ 0x08
DATA_SEG equ 0x10

; Signature BIOS
times 510 - ($ - $$) db 0
dw 0xAA55
