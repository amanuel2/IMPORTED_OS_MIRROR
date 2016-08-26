CFLAGS ?= -Os -s
CFLAGSWARN := -Wall -Wextra
CFLAGSVITAL := -ffreestanding -std=c99 -MD -Iinclude -Iinclude/arch/${ARCHDIR}
CFLAGSALL := ${CFLAGSWARN} ${CFLAGSVITAL} ${CFLAGS}

DESTDIR ?=

OBJECTS := \
	arch/${ARCHDIR}/boot.o \
	arch/${ARCHDIR}/halt.o \
	arch/${ARCHDIR}/idle.o \
	arch/${ARCHDIR}/idt.o \
	arch/${ARCHDIR}/interrupt.o \
	arch/${ARCHDIR}/page.o \
	arch/${ARCHDIR}/paging.o \
	arch/${ARCHDIR}/pic.o \
	arch/${ARCHDIR}/pit.o \
	arch/${ARCHDIR}/portio.o \
	arch/${ARCHDIR}/task.o \
	arch/${ARCHDIR}/task-idle.o \
	arch/${ARCHDIR}/task-start.o \
	arch/${ARCHDIR}/task-switch.o \
	ansicode.o \
	lfb.o \
	log.o \
	main.o \
	mod.o \
	panic.o \
	pff2.o \
	pmm.o \
	ps2.o \
	scheduler.o \
	vmm.o \

-include ${OBJECTS:.o=.d}

.PHONY: all clean install

all: glaux.kernel

clean:
	rm -f ${OBJECTS} ${OBJECTS:.o=.d}
	rm -f glaux.kernel

glaux.kernel: ${OBJECTS}
	${CC} -T arch/${ARCHDIR}/linker.ld -o glaux.kernel \
	${OBJECTS} ${DESTDIR}${LIBDIR}/libk.a \
	${CFLAGSALL} -nostdlib -lgcc

install: glaux.kernel
	mkdir -p ${DESTDIR}${BOOTDIR}
	cp --preserve=timestamps glaux.kernel ${DESTDIR}${BOOTDIR}/glaux.kernel

%.o: %.c
	${CC} -c $< -o $@ ${CFLAGSALL}

%.o: %.s
	${AS} $< -o $@
