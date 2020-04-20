.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager16handlerInterruptEhj
.global _ZN16InterruptManager22IgnoreInterruptRequestEv

.macro HandleException num
.global _ZN16InterruptManager19handlerException\num\()Ev
_ZN16InterruptManager19handlerException\num\()Ev:
    movb $\num, (interruptNumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptNumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01

int_bottom:

    #pusha 
    #pushl %ds
    #pushl %es
    #pushl %fs
    #pushl %gs

    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    pushl %esp
    push (interruptNumber)
    call _ZN16InterruptManager16handlerInterruptEhj

    mov %eax, %esp    

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx
    
    popl %esi
    popl %edi
    popl %ebp

    add $4, %esp

_ZN16InterruptManager22IgnoreInterruptRequestEv:
    iret

.data 
    interruptNumber: .byte 0