/*
 * Copyright (c) 2013, 2014, 2016 Jonas 'Sortie' Termansen.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * input.h
 * Keyboard input.
 */

#ifndef EDITOR_INPUT_H
#define EDITOR_INPUT_H

#include <termios.h>
#include <wchar.h>

#if !defined(__sortix__)
#define KBKEY_ESC 0x01
#define KBKEY_BKSPC 0x0E
#define KBKEY_HOME (0x80 + 0x47)
#define KBKEY_UP (0x80 + 0x48)
#define KBKEY_PGUP (0x80 + 0x49)
#define KBKEY_LEFT (0x80 + 0x4B)
#define KBKEY_RIGHT (0x80 + 0x4D)
#define KBKEY_END (0x80 + 0x4F)
#define KBKEY_DOWN (0x80 + 0x50)
#define KBKEY_PGDOWN (0x80 + 0x51)
#define KBKEY_DELETE (0x80 + 0x53)
#endif

struct editor;

#define MAX_TERMSEQ_SIZE 16

struct editor_input
{
	struct termios saved_termios;
	mbstate_t ps;
	char termseq[MAX_TERMSEQ_SIZE];
	size_t termseq_used;
	size_t termseq_seen;
	bool ambiguous_escape;
};

void editor_input_begin(struct editor_input* editor_input);
void editor_input_process(struct editor_input* editor_input,
                          struct editor* editor);
void editor_input_end(struct editor_input* editor_input);
void editor_input_suspend(struct editor_input* editor_input);

#endif
