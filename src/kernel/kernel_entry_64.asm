[bits 32]
[extern kernel_main]

global _start
_start:
    ; Print message before paging setup
    mov esi, MSG_PAGING
    call print_string_pm
    
    ; Set up paging for 64-bit mode - modified approach
    call setup_simple_paging
    
    ; Print message after paging setup
    mov esi, MSG_GDT
    call print_string_pm
    
    ; Set up the 64-bit GDT
    call setup_64bit_gdt
    
    ; Print message before switching
    mov esi, MSG_LONG_MODE
    call print_string_pm
    
    ; Switch to long mode
    call switch_to_long_mode
    
    ; This point is never reached - the switch jumps to long_mode_start

; Function to print a null-terminated string in 32-bit protected mode
; ESI = pointer to string
print_string_pm:
    pusha
    mov edx, 0xb8000       ; Video memory base
    mov ah, 0x0F           ; White text on black background
.loop:
    lodsb                  ; Load character from ESI into AL
    test al, al            ; Check for end of string (0)
    jz .done               ; If zero, we're done
    mov [edx], ax          ; Store character/attribute at video memory
    add edx, 2             ; Move to next character position
    jmp .loop              ; Continue with next character
.done:
    popa
    ret

; Simplified paging setup - only map the first 2MB
setup_simple_paging:
    ; Define addresses for page tables
    %define PML4_ADDR 0x10000
    %define PDPT_ADDR 0x11000
    %define PD_ADDR   0x12000
    %define PT_ADDR   0x13000
    
    ; Clear the PML4 table
    mov edi, PML4_ADDR
    mov ecx, 1024          ; 4KB / 4 bytes
    xor eax, eax
    rep stosd
    
    mov esi, MSG_PAGING_2
    call print_string_pm
    
    ; Set up PML4 entry (first entry points to PDPT)
    mov edi, PML4_ADDR
    mov eax, PDPT_ADDR
    or eax, 3              ; Present + Writable
    mov [edi], eax
    
    mov esi, MSG_PAGING_3
    call print_string_pm
    
    ; Clear the PDPT
    mov edi, PDPT_ADDR
    mov ecx, 1024          ; 4KB / 4 bytes
    xor eax, eax
    rep stosd
    
    ; Set up PDPT entry (first entry points to PD)
    mov edi, PDPT_ADDR
    mov eax, PD_ADDR
    or eax, 3              ; Present + Writable
    mov [edi], eax
    
    mov esi, MSG_PAGING_4
    call print_string_pm
    
    ; Clear the PD
    mov edi, PD_ADDR
    mov ecx, 1024          ; 4KB / 4 bytes
    xor eax, eax
    rep stosd
    
    ; Set up PD entry (first entry points to PT)
    mov edi, PD_ADDR
    mov eax, PT_ADDR
    or eax, 3              ; Present + Writable
    mov [edi], eax
    
    mov esi, MSG_PAGING_5
    call print_string_pm
    
    ; Clear the PT
    mov edi, PT_ADDR
    mov ecx, 1024          ; 4KB / 4 bytes
    xor eax, eax
    rep stosd
    
    ; Map the first 2MB (512 pages of 4KB each)
    mov edi, PT_ADDR
    mov eax, 0             ; Start physical address
    or eax, 3              ; Present + Writable
    
    mov ecx, 512           ; 512 entries (2MB of memory)
.map_pt_loop:
    mov [edi], eax         ; Write the entry
    add eax, 0x1000        ; Next page (4KB)
    add edi, 8             ; Next entry (8 bytes)
    loop .map_pt_loop
    
    mov esi, MSG_PAGING_6
    call print_string_pm
    
    ; Enable paging by setting CR3 register
    mov eax, PML4_ADDR
    mov cr3, eax
    
    mov esi, MSG_PAGING_7
    call print_string_pm
    
    ret

; Set up the 64-bit GDT
setup_64bit_gdt:
    ; Load GDT register with our GDT
    lgdt [gdt64_pointer]
    ret

; Switch to long mode
switch_to_long_mode:
    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5          ; Set PAE bit
    mov cr4, eax
    
    mov esi, MSG_PAE
    call print_string_pm
    
    ; Set long mode bit in EFER MSR
    mov ecx, 0xC0000080     ; EFER MSR number
    rdmsr                   ; Read EFER
    or eax, 1 << 8          ; Set LM bit
    wrmsr                   ; Write EFER
    
    mov esi, MSG_EFER
    call print_string_pm
    
    ; Enable paging (this automatically enables long mode)
    mov eax, cr0
    or eax, 1 << 31         ; Set PG bit
    mov cr0, eax
    
    mov esi, MSG_PAGING_ENABLE
    call print_string_pm
    
    ; Jump to 64-bit code segment
    jmp CODE_SEG:long_mode_start

[bits 64]
long_mode_start:
    ; Initialize 64-bit segment registers
    xor rax, rax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Print success message directly to screen
    mov rdi, 0xB8000
    mov rcx, 25
    mov rax, 0x1F201F201F201F20  ; Blue background, space
    rep stosq                    ; Clear part of the screen
    
    mov rdi, 0xB8000
    mov rax, 0x1F681F651F6C1F6C  ; "hell" with blue background
    stosq
    mov rax, 0x1F361F361F201F6F  ; "o 64" with blue background
    stosq
    
    ; Set up stack for the C kernel
    mov rsp, 0x90000
    
    ; Call the C kernel
    call kernel_main
    
    ; If kernel returns, halt the CPU
    hlt

; 64-bit GDT
gdt64_start:
    ; Null descriptor
    dq 0
    ; Code segment descriptor
    dw 0xFFFF               ; Limit (bits 0-15)
    dw 0x0000               ; Base (bits 0-15)
    db 0x00                 ; Base (bits 16-23)
    db 10011010b            ; Access byte
    db 10101111b            ; Flags + Limit (bits 16-19)
    db 0x00                 ; Base (bits 24-31)
    ; Data segment descriptor
    dw 0xFFFF               ; Limit (bits 0-15)
    dw 0x0000               ; Base (bits 0-15)
    db 0x00                 ; Base (bits 16-23)
    db 10010010b            ; Access byte
    db 10101111b            ; Flags + Limit (bits 16-19)
    db 0x00                 ; Base (bits 24-31)
gdt64_end:

gdt64_pointer:
    dw gdt64_end - gdt64_start - 1    ; Limit
    dd gdt64_start                    ; Base

; Constants
CODE_SEG equ 8             ; Code segment offset in GDT

; Debug messages
MSG_PAGING db "Setting up paging...", 0
MSG_PAGING_2 db "Paging step 2: PML4 setup", 0
MSG_PAGING_3 db "Paging step 3: PDPT setup", 0
MSG_PAGING_4 db "Paging step 4: PD setup", 0
MSG_PAGING_5 db "Paging step 5: PT setup", 0
MSG_PAGING_6 db "Paging step 6: Memory mapping complete", 0
MSG_PAGING_7 db "Paging step 7: CR3 set", 0
MSG_GDT db "Setting up GDT...", 0
MSG_LONG_MODE db "Switching to long mode...", 0
MSG_PAE db "Enabling PAE...", 0
MSG_EFER db "Setting EFER...", 0
MSG_PAGING_ENABLE db "Enabling paging...", 0