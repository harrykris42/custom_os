[bits 16]
[org 0x7c00]

; Constants
KERNEL_OFFSET equ 0x1000
STACK_BASE equ 0x9000

; Boot entry point
boot_start:
    ; Set up initial environment
    cli                     ; Disable interrupts
    xor ax, ax              ; Zero ax register
    mov ds, ax              ; Set DS=0
    mov es, ax              ; Set ES=0
    mov ss, ax              ; Set SS=0
    mov sp, STACK_BASE      ; Set stack

    ; Enable A20 line
    call enable_a20
    
    ; Check if CPUID is supported
    call check_cpuid
    
    ; Check if long mode is available
    call check_long_mode
    
    ; Load the kernel
    mov si, MSG_LOAD_KERNEL
    call print_string
    call load_kernel
    
    ; Switch to 32-bit protected mode first
    mov si, MSG_PROT_MODE
    call print_string
    jmp switch_to_protected_mode
    
    ; This should never be reached
    jmp $

; Enable A20 line - required to access memory above 1MB
enable_a20:
    ; Use BIOS to enable A20
    mov ax, 0x2401
    int 0x15
    ret

; Check if CPUID instruction is available
check_cpuid:
    ; Try to flip the ID bit (bit 21) in EFLAGS
    pushfd                  ; Save EFLAGS
    pop eax                 ; Store EFLAGS in EAX
    mov ecx, eax            ; Copy to ECX for comparison later
    xor eax, 1 << 21        ; Flip ID bit
    push eax                ; Push modified value
    popfd                   ; Try to set EFLAGS with ID bit flipped
    pushfd                  ; Get EFLAGS again
    pop eax                 ; Store in EAX
    push ecx                ; Restore original EFLAGS
    popfd
    
    ; Compare - if they're the same, CPUID is not supported
    xor eax, ecx
    jz no_cpuid_error
    ret
    
no_cpuid_error:
    mov si, MSG_NO_CPUID
    call print_string
    jmp $

; Check if long mode is available
check_long_mode:
    ; Use CPUID to check for long mode support
    mov eax, 0x80000000     ; Check if extended functions are available
    cpuid
    cmp eax, 0x80000001     ; Is 0x80000001 available?
    jb no_long_mode_error   ; If not, long mode not supported
    
    mov eax, 0x80000001     ; Test for long mode
    cpuid
    test edx, 1 << 29       ; Test LM bit
    jz no_long_mode_error
    ret
    
no_long_mode_error:
    mov si, MSG_NO_LONG_MODE
    call print_string
    jmp $

; Load kernel from disk
load_kernel:
    mov ah, 0x02            ; BIOS read sector function
    mov al, 40              ; Number of sectors to read (increased for 64-bit kernel)
    mov ch, 0               ; Cylinder 0
    mov cl, 2               ; Start from sector 2
    mov dh, 0               ; Head 0
    mov dl, 0x00            ; Drive number (floppy)
    mov bx, KERNEL_OFFSET   ; Buffer to load to
    int 0x13                ; BIOS interrupt
    jc disk_error
    ret
    
disk_error:
    mov si, MSG_DISK_ERROR
    call print_string
    jmp $

; Set up the GDT
gdt_start:
    ; Null descriptor
    dd 0x0
    dd 0x0
    
    ; Code segment descriptor
    dw 0xffff    ; Limit (bits 0-15)
    dw 0x0       ; Base (bits 0-15)
    db 0x0       ; Base (bits 16-23)
    db 10011010b ; Access byte
    db 11001111b ; Flags + Limit (bits 16-19)
    db 0x0       ; Base (bits 24-31)
    
    ; Data segment descriptor
    dw 0xffff    ; Limit (bits 0-15)
    dw 0x0       ; Base (bits 0-15)
    db 0x0       ; Base (bits 16-23)
    db 10010010b ; Access byte
    db 11001111b ; Flags + Limit (bits 16-19)
    db 0x0       ; Base (bits 24-31)
gdt_end:

; GDT descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size (16 bits)
    dd gdt_start                ; Address (32 bits)

; Define segment selectors
CODE_SEG equ 0x08   ; Offset of code segment in GDT
DATA_SEG equ 0x10   ; Offset of data segment in GDT

; Switch to 32-bit protected mode
switch_to_protected_mode:
    cli                     ; Disable interrupts
    lgdt [gdt_descriptor]   ; Load the GDT
    
    ; Enable protected mode by setting CR0 bit 0
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    ; Jump to 32-bit code segment to flush pipeline
    jmp CODE_SEG:start_protected_mode

; 32-bit code
[bits 32]
start_protected_mode:
    ; Initialize segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up 32-bit stack
    mov ebp, 0x90000
    mov esp, ebp
    
    ; Call the second stage to set up long mode
    ; This will be loaded as part of the kernel
    jmp KERNEL_OFFSET

; Utility: Print string (SI = pointer to string)
[bits 16]
print_string:
    pusha
    mov ah, 0x0E            ; BIOS teletype
.loop:
    lodsb                   ; Load byte from SI into AL
    test al, al             ; Check if end of string
    jz .done                ; If zero, we're done
    int 0x10                ; Print character
    jmp .loop               ; Next character
.done:
    popa
    ret

; Messages
MSG_LOAD_KERNEL db 'Loading 64-bit kernel...', 0x0D, 0x0A, 0
MSG_PROT_MODE db 'Switching to protected mode...', 0x0D, 0x0A, 0
MSG_NO_CPUID db 'ERROR: CPUID not supported', 0x0D, 0x0A, 0
MSG_NO_LONG_MODE db 'ERROR: Long mode not supported', 0x0D, 0x0A, 0
MSG_DISK_ERROR db 'ERROR: Failed to load kernel', 0x0D, 0x0A, 0

; Boot sector padding
times 510-($-$$) db 0
dw 0xAA55