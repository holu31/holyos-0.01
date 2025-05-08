CC := clang
CFLAGS := -nostdlib -g -I include -ffreestanding -O0 -Wall -Wextra
ASFLAGS := -g -O0
LDFLAGS := -T boot/linker.ld -Wl,--gc-sections
OUTPUT := kernel.elf
OUTPUT_ISO := myos.iso

override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep 'clang' >/dev/null 2>&1; echo $$?)

ifeq ($(CC_IS_CLANG),1)
    override CC += \
        -target i686-pc-none-elf -march=i686
endif

override SRCFILES := $(shell find -L * -type f | LC_ALL=C sort)
override CFILES := $(filter %.c,$(SRCFILES))
override ASFILES := $(filter %.s,$(SRCFILES))
override OBJ := $(addprefix build/obj/,$(CFILES:.c=.c.o) $(ASFILES:.s=.s.o))

.PHONY: all
all: build/$(OUTPUT)

build/$(OUTPUT): $(OBJ)
	mkdir -p "$$(dirname $@)"
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJ) -o $@

build/obj/%.c.o: %.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) -c $< -o $@

build/obj/%.s.o: %.s
	mkdir -p "$$(dirname $@)"
	$(CC) $(ASFLAGS) -m32 -c $< -o $@

iso: build/$(OUTPUT)
	cp build/$(OUTPUT) isodir/boot/$(OUTPUT)
	grub-mkrescue -o build/$(OUTPUT_ISO) isodir

qemu: iso
	qemu-system-i386 -cdrom build/$(OUTPUT_ISO) -serial stdio

qemu-gdb: iso
	qemu-system-i386 -m 125M -cpu max -serial stdio -cdrom build/$(OUTPUT_ISO) -s -S &
	gdb -ex "file build/${OUTPUT}" -ex "target remote localhost:1234"

.PHONY: clean
clean:
	rm -rf build isodir/boot/$(OUTPUT)
