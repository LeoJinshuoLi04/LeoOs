AS = i686-elf-as
CPP = i686-elf-g++
LD = i686-elf-gcc

CPPFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
LDFLAGS = -T src/linker.ld -ffreestanding -O2 -nostdlib -lgcc

# Create the build directory and compile everything
all:
	mkdir -p build
	$(AS) src/boot.s -o build/boot.o
	$(AS) src/interrupts.s -o build/interrupts.o
	$(CPP) $(CPPFLAGS) -c src/terminal.cpp -o build/terminal.o
	$(CPP) $(CPPFLAGS) -c src/kernel.cpp -o build/kernel.o
	$(CPP) $(CPPFLAGS) -c src/gdt.cpp -o build/gdt.o
	$(CPP) $(CPPFLAGS) -c src/isr.cpp -o build/isr.o
	$(CPP) $(CPPFLAGS) -c src/idt.cpp -o build/idt.o
	$(CPP) $(CPPFLAGS) -c src/pic.cpp -o build/pic.o
	$(LD) $(LDFLAGS) -o build/myos.bin build/boot.o build/interrupts.o build/terminal.o build/kernel.o build/gdt.o build/isr.o build/idt.o build/pic.o

run: all
	qemu-system-i386 -kernel build/myos.bin

clean:
	rm -rf build