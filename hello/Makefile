CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BINDIR?=$(EXEC_PREFIX)/bin

CFLAGS:=$(CFLAGS) -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS)

all: hello

.PHONY: all clean install

hello: hello.c
	$(CC) $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LIBS)

clean:
	rm -f hello

install: hello
	mkdir -p $(DESTDIR)$(BINDIR)
	cp hello $(DESTDIR)$(BINDIR)
