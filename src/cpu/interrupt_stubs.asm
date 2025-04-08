[bits 64]

; Export our handler functions
global isr_common_stub
global irq_common_stub

; Export our ISR handlers
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push 0      ; Push dummy error code
    push %1     ; Push interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    ; Error code is already pushed by CPU
    push %1     ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Export our IRQ handlers
%macro IRQ 2
global irq%1
irq%1:
    push 0      ; Push dummy error code
    push %2     ; Push interrupt number
    jmp irq_common_stub
%endmacro

; Define ISRs
ISR_NOERRCODE 0   ; Divide by zero
ISR_NOERRCODE 1   ; Debug
ISR_NOERRCODE 2   ; Non-maskable interrupt
ISR_NOERRCODE 3   ; Breakpoint
ISR_NOERRCODE 4   ; Overflow
ISR_NOERRCODE 5   ; Bound range exceeded
ISR_NOERRCODE 6   ; Invalid opcode
ISR_NOERRCODE 7   ; Device not available
ISR_ERRCODE   8   ; Double fault
ISR_NOERRCODE 9   ; Coprocessor segment overrun
ISR_ERRCODE   10  ; Invalid TSS
ISR_ERRCODE   11  ; Segment not present
ISR_ERRCODE   12  ; Stack-segment fault
ISR_ERRCODE   13  ; General protection fault
ISR_ERRCODE   14  ; Page fault
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 floating-point exception
ISR_ERRCODE   17  ; Alignment check
ISR_NOERRCODE 18  ; Machine check
ISR_NOERRCODE 19  ; SIMD floating-point exception
ISR_NOERRCODE 20  ; Virtualization exception
ISR_NOERRCODE 21  ; Reserved
ISR_NOERRCODE 22  ; Reserved
ISR_NOERRCODE 23  ; Reserved
ISR_NOERRCODE 24  ; Reserved
ISR_NOERRCODE 25  ; Reserved
ISR_NOERRCODE 26  ; Reserved
ISR_NOERRCODE 27  ; Reserved
ISR_NOERRCODE 28  ; Reserved
ISR_NOERRCODE 29  ; Reserved
ISR_NOERRCODE 30  ; Reserved
ISR_NOERRCODE 31  ; Reserved

; Define IRQs
IRQ 0, 32   ; Timer
IRQ 1, 33   ; Keyboard
IRQ 2, 34   ; Cascade for 8259A Slave controller
IRQ 3, 35   ; Serial port 2
IRQ 4, 36   ; Serial port 1
IRQ 5, 37   ; Parallel port 2/3 or sound card
IRQ 6, 38   ; Floppy disk controller
IRQ 7, 39   ; Parallel port 1
IRQ 8, 40   ; Real-time clock
IRQ 9, 41   ; Legacy SCSI or NIC
IRQ 10, 42  ; Available
IRQ 11, 43  ; Available
IRQ 12, 44  ; PS/2 mouse
IRQ 13, 45  ; FPU, coprocessor or inter-processor
IRQ 14, 46  ; Primary ATA channel
IRQ 15, 47  ; Secondary ATA channel

; Import our C handlers
extern isr_handler
extern irq_handler

; Common ISR handler
isr_common_stub:
    ; Save all registers
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Save data segment
    mov ax, ds
    push rax

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler
    mov rdi, rsp    ; Pass pointer to registers as argument
    call isr_handler

    ; Restore data segment
    pop rax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Restore registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    ; Remove error code and interrupt number
    add rsp, 16
    iretq

; Common IRQ handler
irq_common_stub:
    ; Save all registers
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Save data segment
    mov ax, ds
    push rax

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler
    mov rdi, rsp    ; Pass pointer to registers as argument
    call irq_handler

    ; Restore data segment
    pop rax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Restore registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    ; Remove error code and interrupt number
    add rsp, 16
    iretq