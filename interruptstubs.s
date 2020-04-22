.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager16HandlerInterruptEhj

.macro HandleException num
.global _ZN16InterruptManager19HandlerException\num\()Ev
_ZN16InterruptManager19HandlerException\num\()Ev:
    movb $\num, (interruptNumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptNumber)
    pushl $0
    jmp int_bottom
.endm


HandleInterruptRequest 0x00
HandleInterruptRequest 0x01

int_bottom:

    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    
    pushl %esp
    push (interruptNumber)
    call _ZN16InterruptManager16HandlerInterruptEhj

    mov %eax, %esp   


    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa


.global _ZN16InterruptManager22IgnoreInterruptRequestEv
_ZN16InterruptManager22IgnoreInterruptRequestEv:
    iret

.data 
    interruptNumber: .byte 0