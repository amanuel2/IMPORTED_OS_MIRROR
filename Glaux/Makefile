HOST ?= i686-elf

ifeq (${HOST}, i386-elf)
	MACHINE := i386
	ARCHDIR := x86
endif

ifeq (${HOST}, i486-elf)
	MACHINE := i486
	ARCHDIR := x86
endif

ifeq (${HOST}, i586-elf)
	MACHINE := i586
	ARCHDIR := x86
endif

ifeq (${HOST}, i686-elf)
	MACHINE := i686
	ARCHDIR := x86
endif

BOOTDIR := /boot
PREFIX ?= /usr
INCLUDEDIR := ${PREFIX}/include
LIBDIR := ${PREFIX}/lib
MANDIR := ${PREFIX}/man

SYSROOT ?= ${shell pwd}/sysroot
GLAUXISO ?= ${shell pwd}/glaux.iso

CC := ${HOST}-gcc
CC := ${CC} --sysroot=${SYSROOT} -isystem=${INCLUDEDIR}
AR := ${HOST}-ar
AS := ${HOST}-as

export HOST
export MACHINE
export ARCHDIR

export BOOTDIR
export PREFIX
export INCLUDEDIR
export LIBDIR
export MANDIR

export CC
export AR
export AS

.PHONY: all clean install iso

all:
	${MAKE} -C libc all
	${MAKE} -C kernel all

clean:
	${MAKE} -C libc clean
	${MAKE} -C kernel clean

install:
	mkdir -p ${SYSROOT}${BOOTDIR}
	cp -RT --preserve=timestamps boot ${SYSROOT}${BOOTDIR}
	${MAKE} -C libc install-headers DESTDIR=${SYSROOT}
	${MAKE} -C libc install-man DESTDIR=${SYSROOT}
	${MAKE} -C libc install DESTDIR=${SYSROOT}
	${MAKE} -C kernel install DESTDIR=${SYSROOT}

iso: install
	grub-mkrescue -o ${GLAUXISO} ${SYSROOT}
