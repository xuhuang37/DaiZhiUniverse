#include "types.h"
#include "interrupts.h"

void InterruptManager::SetInterruptDescriptorTableEntry(
        uint8_t interrruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void(*handler)(),
        uint8_t DescriptorPrivilegeLevel,
        uint8_t DescriptorType)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interrruptNumber].handlerAddressLowBits = ((uint32_t)handler)&0xFFF;
    interruptDescriptorTable[interrruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFF;
    interruptDescriptorTable[interrruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interrruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel&3)<<5);
    interruptDescriptorTable[interrruptNumber].handlerAddressLowBits = ((uint32_t)handler)&0xFFF;
    interruptDescriptorTable[interrruptNumber].reserved = 0;
    
}

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager::InterruptManager(GlobalDescriptorTable* gdt){
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for (uint16_t i = 0; i < 256; i++)
    {
        /* code */
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest,0,IDT_INTERRUPT_GATE);
    }

    SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01,0,IDT_INTERRUPT_GATE);

    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));
    
}

InterruptManager::~InterruptManager(){}

void InterruptManager::Activate(){

}

uint32_t InterruptManager::handlerInterrupt(uint8_t interruptNumber, uint32_t esp)
{
    return esp;
}