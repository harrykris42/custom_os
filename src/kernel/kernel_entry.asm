[bits 32]
[extern main]     ; Declare that we will be referencing the external symbol 'main'
call main         ; Call our main() function
jmp $             ; Hang if main returns