#include "types.h"
#include "interrupts.h"


void printf(const int8_t* str);

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager* InterruptManager::ActiveInterruptManager =  0;

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
    interruptDescriptorTable[interrruptNumber].reserved = 0;
    
}

InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
:picMasterCommand(0x20),
picMasterData(0x21),
picSlaveCommand(0xA0),
picSlaveData(0xA1)
{
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for (uint16_t i = 0; i < 256; i++)
    {
        /* code */
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest,0,IDT_INTERRUPT_GATE);
    }

    SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01,0,IDT_INTERRUPT_GATE);

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    picMasterData.Write(0x20);
    picSlaveData.Write(0x28);
    
    picMasterData.Write(0x40);
    picSlaveData.Write(0x28);
    
    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);
    
    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);
    

    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    __asm__ volatile("lidt %0" : : "m" (idt));
    
}

void InterruptManager::Activate(){
    if (ActiveInterruptManager != 0)
        ActiveInterruptManager->Deactivate();    
    ActiveInterruptManager = this;
    asm("sti");
}

void InterruptManager::Deactivate(){
    if (ActiveInterruptManager == this){
        ActiveInterruptManager = 0;
        asm("cli");
    }
}

uint32_t InterruptManager::HandlerInterrupt(uint8_t interruptNumber, uint32_t esp)
{
    if (ActiveInterruptManager != 0){}
    {
        return ActiveInterruptManager->DoHandlerInterrupt(interruptNumber,esp);
    }
    return esp;
}

uint32_t InterruptManager::DoHandlerInterrupt(uint8_t interruptNumber, uint32_t esp)
{
    printf("INTERRUPT");
    if (0x20 <= interruptNumber && interruptNumber < 0x30){}
    {
        picMasterCommand.Write(0x20);
        if (0x28<= interruptNumber)
            picSlaveCommand.Write(0x11);
            /* code */
        
    }
    return esp;
}