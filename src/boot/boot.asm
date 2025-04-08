[bits 16]
[org 0x7c00]

KERNEL_OFFSET equ 0x1000   ; Memory offset to which we'll load our kernel

; Initialize segment registers
mov ax, 0x0000
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; Print a message
mov si, MSG_REAL_MODE
call print_string

; Load the kernel from disk
call load_kernel

; Print a message before switching to protected mode
mov si, MSG_PROT_MODE
call print_string

; Switch to 32-bit protected mode
call switch_to_pm

; We never return from switch_to_pm
jmp $

; Loads the kernel into memory
load_kernel:
    mov si, MSG_LOAD_KERNEL
    call print_string

    mov ah, 0x02      ; BIOS read sector function
    mov al, 30        ; Number of sectors to read (increase this to be safe)
    mov ch, 0         ; Cylinder number
    mov cl, 2         ; Sector number (kernel starts at sector 2)
    mov dh, 0         ; Head number
    mov dl, 0x00      ; Drive number (0x00 for floppy)
    
    ; Load to ES:BX = 0x0000:KERNEL_OFFSET
    mov bx, KERNEL_OFFSET
    
    int 0x13          ; BIOS interrupt to read sectors
    jc disk_error     ; Jump if error (carry flag set)
    
    ret

disk_error:
    mov si, MSG_DISK_ERROR
    call print_string
    jmp $

; Print a string (SI = string pointer)
print_string:
    pusha
.loop:
    lodsb               ; Load byte from SI into AL
    or al, al           ; Check if AL is 0 (end of string)
    jz .done            ; If zero, we're done
    mov ah, 0x0E        ; BIOS teletype function
    int 0x10            ; Call BIOS interrupt
    jmp .loop           ; Repeat for next character
.done:
    popa
    ret

; GDT (Global Descriptor Table)
gdt_start:

gdt_null:        ; The mandatory null descriptor
    dd 0x0       ; 4 bytes of zeros
    dd 0x0

gdt_code:        ; Code segment descriptor
    dw 0xffff    ; Limit (bits 0-15)
    dw 0x0       ; Base (bits 0-15)
    db 0x0       ; Base (bits 16-23)
    db 10011010b ; Flags (8 bits)
    db 11001111b ; Flags (4 bits) + limit (bits 16-19)
    db 0x0       ; Base (bits 24-31)

gdt_data:        ; Data segment descriptor
    dw 0xffff    ; Limit (bits 0-15)
    dw 0x0       ; Base (bits 0-15)
    db 0x0       ; Base (bits 16-23)
    db 10010010b ; Flags (8 bits)
    db 11001111b ; Flags (4 bits) + limit (bits 16-19)
    db 0x0       ; Base (bits 24-31)

gdt_end:         ; Used to calculate GDT size for the descriptor

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of the GDT
    dd gdt_start                ; Address of the GDT

; Constants for segment descriptors
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Switch to 32-bit protected mode
switch_to_pm:
    cli                    ; Disable interrupts
    lgdt [gdt_descriptor]  ; Load the GDT
    
    ; Set PE (Protection Enable) bit in CR0
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    ; Far jump to 32-bit code
    jmp CODE_SEG:init_pm
    
[bits 32]
; Initialize 32-bit protected mode
init_pm:
    ; Update segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up a new stack
    mov ebp, 0x90000
    mov esp, ebp
    
    ; Jump to the kernel
    call KERNEL_OFFSET
    jmp $

[bits 16]
; Messages
MSG_REAL_MODE db 'Started in 16-bit Real Mode', 0x0D, 0x0A, 0
MSG_LOAD_KERNEL db 'Loading kernel into memory...', 0x0D, 0x0A, 0
MSG_PROT_MODE db 'Switching to protected mode...', 0x0D, 0x0A, 0
MSG_DISK_ERROR db 'Disk read error!', 0x0D, 0x0A, 0

; Padding and boot signature
times 510-($-$$) db 0   ; Pad with zeros until 510 bytes
dw 0xAA55               ; Boot signature