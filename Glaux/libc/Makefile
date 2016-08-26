CFLAGS ?= -Os -s
CFLAGSWARN := -Wall -Wextra
CFLAGSVITAL := -std=c99 -MD -Iinclude
CFLAGSALL := ${CFLAGSWARN} ${CFLAGSVITAL} ${CFLAGS}

CFLAGS_LIBK := ${CFLAGSALL} -ffreestanding
CFLAGS_LIBC := ${CFLAGSALL} -ffreestanding

DESTDIR ?=

DIRS_LIBK := \
	libk/string/${ARCHDIR} \
	libk/stdio/${ARCHDIR} \
	libk/wchar/${ARCHDIR} \

DIRS_LIBC := \
	libc/string/${ARCHDIR} \
	libc/stdio/${ARCHDIR} \
	libc/wchar/${ARCHDIR} \

OBJECTS_LIBK := \
	libk/string/${ARCHDIR}/memcmp.o \
	libk/string/${ARCHDIR}/memmove.o \
	libk/string/${ARCHDIR}/memset.o \
	libk/string/strcat.o \
	libk/string/strcmp.o \
	libk/string/strcpy.o \
	libk/string/strlcat.o \
	libk/string/strlcpy.o \
	libk/string/strlen.o \
	libk/string/strncat.o \
	libk/string/strnlen.o \
	libk/string/strncpy.o \
	libk/stdio/cbprintf.o \
	libk/stdio/snprintf.o \
	libk/stdio/vsnprintf.o \
	libk/stdio/vcbprintf.o \
	libk/wchar/mbrtowc.o \

OBJECTS_LIBC := \
	libc/string/${ARCHDIR}/memcmp.o \
	libc/string/${ARCHDIR}/memmove.o \
	libc/string/${ARCHDIR}/memset.o \
	libc/string/strcat.o \
	libc/string/strcmp.o \
	libc/string/strcpy.o \
	libc/string/strlcat.o \
	libc/string/strlcpy.o \
	libc/string/strlen.o \
	libc/string/strncat.o \
	libc/string/strncpy.o \
	libc/string/strnlen.o \
	libc/stdio/cbprintf.o \
	libc/stdio/snprintf.o \
	libc/stdio/vsnprintf.o \
	libc/stdio/vcbprintf.o \
	libc/wchar/mbrtowc.o \

-include ${OBJECTS_LIBC:.o=.d}
-include ${OBJECTS_LIBK:.o=.d}

.PHONY: all clean install install-headers install-man

all: libk.a libc.a

clean:
	rm -f libk.a libc.a ${OBJECTS_LIBK} ${OBJECTS_LIBC}
	rm -f ${OBJECTS_LIBK:.o=.d} ${OBJECTS_LIBC:.o=.d}
	rm -rf libc
	rm -rf libk

install: libk.a libc.a
	mkdir -p ${DESTDIR}${LIBDIR}
	cp --preserve=timestamps libk.a ${DESTDIR}${LIBDIR}/libk.a
	cp --preserve=timestamps libc.a ${DESTDIR}${LIBDIR}/libc.a

install-headers:
	mkdir -p ${DESTDIR}${INCLUDEDIR}
	cp -RT --preserve=timestamps include/common ${DESTDIR}${INCLUDEDIR}
	cp -RT --preserve=timestamps include/${ARCHDIR} ${DESTDIR}${INCLUDEDIR}

install-man:
	mkdir -p ${DESTDIR}${MANDIR}
	cp -RT --preserve=timestamps man ${DESTDIR}${MANDIR}

libk.a: ${DIRS_LIBK} ${OBJECTS_LIBK}
	${AR} rcs libk.a ${OBJECTS_LIBK}

libc.a: ${DIRS_LIBC} ${OBJECTS_LIBC}
	${AR} rcs libc.a ${OBJECTS_LIBC}

libk/%.o: %.c
	${CC} -c $< -o $@ ${CFLAGS_LIBK}

libk/%.o: %.s
	${AS} $< -o $@

libc/%.o: %.c
	${CC} -c $< -o $@ ${CFLAGS_LIBC}

libc/%.o: %.s
	${AS} $< -o $@

${DIRS_LIBK}:
	mkdir -p $@

${DIRS_LIBC}:
	mkdir -p $@
