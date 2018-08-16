extern interrupt_handler

section .text

global load_idt
; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the first entry in the IDT
;        [esp    ] the return address
load_idt:
  mov     eax, [esp+4]    ; load the address of the IDT into register eax
  lidt    [eax]             ; load the IDT
  ret                     ; return to the calling function

global enable_interrupts
enable_interrupts:
  sti
  ret

global disable_interrupts
disable_interrupts:
  cli
  ret

%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
  push dword 0 ; push 0 as error code
  push dword %1 ; push the interrupt number
  jmp common_interrupt_handler ; jump to the common handler
%endmacro

%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
  push dword %1 ; push the interrupt number
  jmp common_interrupt_handler ; jump to the common handler
%endmacro

common_interrupt_handler: ; the common parts of the generic interrupt handler
  ; save the registers
  push eax
  push ebx
  push ecx
  push edx
  push ebp

  ; call the C function
  call interrupt_handler

  ; restore the registers
  pop ebp
  pop edx
  pop ecx
  pop ebx
  pop eax

  ; restore the esp
  add esp, 8

  ; return to the code that got interrupted
  iret

; protected mode exceptions
no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler    8
no_error_code_interrupt_handler 9
error_code_interrupt_handler    10
error_code_interrupt_handler    11
error_code_interrupt_handler    12
error_code_interrupt_handler    13
error_code_interrupt_handler    14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
error_code_interrupt_handler    17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19

; irqs
no_error_code_interrupt_handler 32 ; timer interrupt
no_error_code_interrupt_handler 33 ; keyboard interrupt

global test_divide_by_zero
test_divide_by_zero:
  xor bx, bx
	div bx
	ret

global test_double_fault
test_double_fault:
	int 0x99
	ret
