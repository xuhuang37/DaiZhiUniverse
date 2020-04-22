GPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o gdt.o port.o interrupts.o interruptstubs.o kernel.o

%.o: %.cpp
	g++ $(GPPARAMS) -c -o $@ $<
%.o: %.s
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)
	# rm -f $(objects)



mykernel.iso: mykernel.bin
	rm -rf iso
	mkdir -p iso/boot/grub
	cp mykernel.bin iso/boot/mykernel.bin
	echo 'set timeout=0' 						 > iso/boot/grub/grub.cfg
	echo 'set default=0' 						>> iso/boot/grub/grub.cfg
	echo '' 									>> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' 	>> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/mykernel.bin' 		>> iso/boot/grub/grub.cfg
	echo '	boot' 								>> iso/boot/grub/grub.cfg
	echo '}' 									>> iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ iso
	rm -rf iso

run: mykernel.iso
	# (killall VirtualBox && sleep 1) || true
	# VBoxManage startvm "DaiZhiUniverse" &
	qemu-system-i386 -m 2048 -cdrom mykernel.iso -hda ~/qemu-vm/myos -boot d -d in_asm -monitor stdio -D ./debug/q.log -serial null -s -S & sleep 2
	mate-terminal -e "gdb -q -x ./tools/gdbinit"
	
install: mykernel.bin
	sudo cp $< /boot/mykernel.bin


	
.PHONY: clean
clean:
	rm -f $(objects) mykernel.bin mykernel.iso