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
 */

#if defined(__sortix__)
#include <sys/keycodes.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

#include "command.h"
#include "editor.h"
#include "input.h"
#include "modal.h"

struct terminal_sequence
{
	const char* sequence;
	int kbkey;
	bool control;
	bool shift;
};

// TODO: The terminal doesn't deliver shift-pageup and shift-pagedown events.
// TODO: The terminal doesn't deliver shift-home and shift-end events.

struct terminal_sequence terminal_sequences[] =
{
	{ "\e[1;2A", KBKEY_UP, false, true },
	{ "\e[1;2B", KBKEY_DOWN, false, true },
	{ "\e[1;2C", KBKEY_RIGHT, false, true },
	{ "\e[1;2D", KBKEY_LEFT, false, true },
	{ "\e[1;2F", KBKEY_END, false, true },
	{ "\e[1;2H", KBKEY_HOME, false, true },
	{ "\e[1;2~", KBKEY_HOME, false, true },
	{ "\e[1;5A", KBKEY_UP, true, false },
	{ "\e[1;5B", KBKEY_DOWN, true, false },
	{ "\e[1;5C", KBKEY_RIGHT, true, false },
	{ "\e[1;5D", KBKEY_LEFT, true, false },
	{ "\e[1;5F", KBKEY_END, true, false },
	{ "\e[1;5H", KBKEY_HOME, true, false },
	{ "\e[1;5~", KBKEY_HOME, true, false },
	{ "\e[1;6A", KBKEY_UP, true, true },
	{ "\e[1;6B", KBKEY_DOWN, true, true },
	{ "\e[1;6C", KBKEY_RIGHT, true, true },
	{ "\e[1;6D", KBKEY_LEFT, true, true },
	{ "\e[1;6F", KBKEY_END, true, true },
	{ "\e[1;6H", KBKEY_HOME, true, true },
	{ "\e[1;6~", KBKEY_HOME, true, true },
	{ "\e[1~", KBKEY_HOME, false, false },
	{ "\e[3;2~", KBKEY_DELETE, false, true },
	{ "\e[3;5~", KBKEY_DELETE, true, false },
	{ "\e[3;6~", KBKEY_DELETE, true, true },
	{ "\e[3~", KBKEY_DELETE, false, false },
	{ "\e[4;2~", KBKEY_END, false, true },
	{ "\e[4;5~", KBKEY_END, true, false },
	{ "\e[4;6~", KBKEY_END, true, true },
	{ "\e[4~", KBKEY_END, false, false },
	{ "\e[5;2~", KBKEY_PGUP, false, true },
	{ "\e[5;5~", KBKEY_PGUP, true, false },
	{ "\e[5;6~", KBKEY_PGUP, true, true },
	{ "\e[5~", KBKEY_PGUP, false, false },
	{ "\e[6;2~", KBKEY_PGDOWN, false, true },
	{ "\e[6;5~", KBKEY_PGDOWN, true, false },
	{ "\e[6;6~", KBKEY_PGDOWN, true, true },
	{ "\e[6~", KBKEY_PGDOWN, false, false },
	{ "\e[A", KBKEY_UP, false, false },
	{ "\e[B", KBKEY_DOWN, false, false },
	{ "\e[C", KBKEY_RIGHT, false, false },
	{ "\e[D", KBKEY_LEFT, false, false },
	{ "\e[F", KBKEY_END, false, false },
	{ "\e[H", KBKEY_HOME, false, false },
	{ "\e:", KBKEY_ESC, false, false },
	{ "\eOF", KBKEY_END, false, false },
	{ "\eOH", KBKEY_HOME, false, false },
	{ "\x7F", KBKEY_BKSPC, false, false },
};

void editor_codepoint(struct editor* editor, uint32_t codepoint)
{
	wchar_t c = (wchar_t) codepoint;

	if ( c == L'\b' || c == 127 /* delete */ )
		return;

	if ( editor->mode == MODE_EDIT )
		editor_type_character(editor, c);
	else
		editor_modal_character(editor, c);
}

void editor_type_kbkey(struct editor* editor, int kbkey)
{
	if ( kbkey < 0 )
		return;

	if ( kbkey == KBKEY_ESC )
	{
		editor_type_command(editor);
		return;
	}

	if ( editor->control && editor->shift )
	{
		switch ( kbkey )
		{
		case KBKEY_LEFT: editor_type_control_select_left(editor); break;
		case KBKEY_RIGHT: editor_type_control_select_right(editor); break;
		case KBKEY_UP: editor_type_control_select_up(editor); break;
		case KBKEY_DOWN: editor_type_control_select_down(editor); break;
		}
	}
	else if ( editor->control && !editor->shift )
	{
		switch ( kbkey )
		{
		case KBKEY_LEFT: editor_type_control_left(editor); break;
		case KBKEY_RIGHT: editor_type_control_right(editor); break;
		case KBKEY_UP: editor_type_control_up(editor); break;
		case KBKEY_DOWN: editor_type_control_select_down(editor); break;
		}
	}
	else if ( !editor->control && editor->shift )
	{
		switch ( kbkey )
		{
		case KBKEY_LEFT: editor_type_select_left(editor); break;
		case KBKEY_RIGHT: editor_type_select_right(editor); break;
		case KBKEY_UP: editor_type_select_up(editor); break;
		case KBKEY_DOWN: editor_type_select_down(editor); break;
		case KBKEY_HOME: editor_type_select_home(editor); break;
		case KBKEY_END: editor_type_select_end(editor); break;
		case KBKEY_PGUP: editor_type_select_page_up(editor); break;
		case KBKEY_PGDOWN: editor_type_select_page_down(editor); break;
		case KBKEY_BKSPC: editor_type_backspace(editor); break;
		case KBKEY_DELETE: editor_type_delete(editor); break;
		}
	}
	else if ( !editor->control && !editor->shift )
	{
		switch ( kbkey )
		{
		case KBKEY_LEFT: editor_type_left(editor); break;
		case KBKEY_RIGHT: editor_type_right(editor); break;
		case KBKEY_UP: editor_type_up(editor); break;
		case KBKEY_DOWN: editor_type_down(editor); break;
		case KBKEY_HOME: editor_type_home(editor); break;
		case KBKEY_END: editor_type_end(editor); break;
		case KBKEY_PGUP: editor_type_page_up(editor); break;
		case KBKEY_PGDOWN: editor_type_page_down(editor); break;
		case KBKEY_BKSPC: editor_type_backspace(editor); break;
		case KBKEY_DELETE: editor_type_delete(editor); break;
		}
	}
}

void editor_modal_kbkey(struct editor* editor, int kbkey)
{
	if ( editor->control )
		return;

	if ( kbkey < 0 )
		return;

	switch ( kbkey )
	{
	case KBKEY_LEFT: editor_modal_left(editor); break;
	case KBKEY_RIGHT: editor_modal_right(editor); break;
	case KBKEY_HOME: editor_modal_home(editor); break;
	case KBKEY_END: editor_modal_end(editor); break;
	case KBKEY_BKSPC: editor_modal_backspace(editor); break;
	case KBKEY_DELETE: editor_modal_delete(editor); break;
	case KBKEY_ESC: editor_type_edit(editor); break;
	}
}

void editor_kbkey(struct editor* editor, int kbkey)
{
	if ( editor->mode == MODE_EDIT )
		editor_type_kbkey(editor, kbkey);
	else
		editor_modal_kbkey(editor, kbkey);
}

void editor_emulate_kbkey(struct editor* editor,
                          int kbkey,
                          bool control,
                          bool shift)
{
	editor->control = control;
	editor->lshift = shift;
	editor->rshift = false;
	editor->shift = shift;

	editor_kbkey(editor, kbkey);
	editor_kbkey(editor, -kbkey);

	editor->control = false;
	editor->lshift = false;
	editor->rshift = false;
	editor->shift = false;
}

void editor_emulate_control_letter(struct editor* editor, uint32_t c)
{
#if !defined(__sortix__)
	if ( c == 'Z' )
	{
		raise(SIGSTOP);
	}
#endif

	editor->control = true;
	editor_codepoint(editor, c);
	editor->control = false;
}

void editor_input_begin(struct editor_input* editor_input)
{
	memset(editor_input, 0, sizeof(*editor_input));

	tcgetattr(0, &editor_input->saved_termios);
	struct termios tcattr;
	memcpy(&tcattr, &editor_input->saved_termios, sizeof(struct termios));
	tcattr.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	tcattr.c_iflag |= ICRNL;
	tcattr.c_cc[VMIN] = 1;
	tcattr.c_cc[VTIME] = 0;
	tcsetattr(0, TCSADRAIN, &tcattr);
	if ( getenv("TERM") && strcmp(getenv("TERM"), "sortix") != 0 )
	{
		printf("\e[?1049h");
		fflush(stdout);
	}
}

void editor_input_process(struct editor_input* editor_input,
                          struct editor* editor)
{
	bool was_ambiguous_escape = editor_input->ambiguous_escape;
	editor_input->ambiguous_escape = false;

	if ( was_ambiguous_escape )
		fcntl(0, F_SETFL, fcntl(0, F_GETFL, (void*) NULL) | O_NONBLOCK);

	unsigned char uc;
	ssize_t amount_read = read(0, &uc, sizeof(uc));

	if ( was_ambiguous_escape )
		fcntl(0, F_SETFL, fcntl(0, F_GETFL, (void*) NULL) &~ O_NONBLOCK);

	if ( amount_read != sizeof(uc) )
	{
		if ( was_ambiguous_escape &&
		     (errno == EWOULDBLOCK || errno == EAGAIN) )
			uc = ':';
		else
			return;
	}

#if 0
	if ( 1 <= uc && uc <= 26 )
		fprintf(stderr, "Input: ^%c\n", 'A' + uc - 1);
	else if ( uc == '\e' )
		fprintf(stderr, "Input: ^[\n");
	else
		fprintf(stderr, "Input: '%c'\n", uc);
#endif
#if 0
	fputc(uc, stderr);
#endif

	if ( editor_input->termseq_used < MAX_TERMSEQ_SIZE )
		editor_input->termseq[editor_input->termseq_used++] = (char) uc;

	size_t num_seqs = sizeof(terminal_sequences) / sizeof(terminal_sequences[0]);

	while ( editor_input->termseq_seen < editor_input->termseq_used )
	{
		size_t match = 0;
		size_t match_size = 0;
		bool full_match = false;
		bool partial_match = false;

		for ( size_t i = 0; i < num_seqs; i++ )
		{
			struct terminal_sequence* terminal_sequence = &terminal_sequences[i];
			const char* sequence = terminal_sequence->sequence;

			bool potential_partial_match = false;
			for ( size_t n = 0; n < editor_input->termseq_used; n++ )
			{
				if ( sequence[n] != editor_input->termseq[n] )
				{
					potential_partial_match = false;
					break;
				}

				if ( sequence[n+1] == '\0' )
				{
					potential_partial_match = false;
					full_match = true;
					match = i;
					match_size = n + 1;
					break;
				}

				potential_partial_match = true;
			}

			if ( potential_partial_match )
				partial_match = true;
		}

		if ( full_match )
		{
			editor_emulate_kbkey(editor,
				terminal_sequences[match].kbkey,
				terminal_sequences[match].control,
				terminal_sequences[match].shift);
			memmove(editor_input->termseq,
			        editor_input->termseq + match_size,
			        editor_input->termseq_used - match_size);
			editor_input->termseq_used -= match_size;
			editor_input->termseq_seen = 0;
			continue;
		}

		if ( partial_match )
		{
			editor_input->termseq_seen = editor_input->termseq_used;

			// HACK: We can't reliably tell an actual escape press apart from
			//       the beginning of an escape sequence. However, we could use
			//       timing to get close to the truth, through the assumption
			//       that a following non-blocking read will fail only if this
			//       was a single escape press.
			if ( editor_input->termseq_used == 1 &&
			     editor_input->termseq[0] == '\e' )
			{
				editor_input->ambiguous_escape = true;
				return editor_input_process(editor_input, editor);
			}

			continue;
		}

		char input = editor_input->termseq[0];

		if ( 1 <= input && input <= 26 && input != '\t' && input != '\n' )
		{
			editor_emulate_control_letter(editor, L'A' + input - 1);
		}
		else
		{
			wchar_t wc;
			size_t amount = mbrtowc(&wc, &input, 1, &editor_input->ps);
			if ( amount == (size_t) -1 )
				memset(&editor_input->ps, 0, sizeof(editor_input->ps));
			if ( amount == (size_t)  1 )
				editor_codepoint(editor, (uint32_t) wc);
		}

		memmove(editor_input->termseq,
		        editor_input->termseq + 1,
		        editor_input->termseq_used - 1);
		editor_input->termseq_used--;
		editor_input->termseq_seen = 0;
	}
}

void editor_input_end(struct editor_input* editor_input)
{
	if ( getenv("TERM") && strcmp(getenv("TERM"), "sortix") != 0 )
	{
		printf("\e[?1049l");
		fflush(stdout);
	}
	tcsetattr(0, TCSADRAIN, &editor_input->saved_termios);
}

void editor_input_suspend(struct editor_input* editor_input)
{
	(void) editor_input;

#if !defined(__sortix__)
	struct termios current_termios;

	if ( getenv("TERM") && strcmp(getenv("TERM"), "sortix") != 0 )
	{
		printf("\e[?1049l");
		fflush(stdout);
	}

	tcgetattr(0, &current_termios);

	raise(SIGSTOP);

	tcsetattr(0, TCSADRAIN, &current_termios);

	if ( getenv("TERM") && strcmp(getenv("TERM"), "sortix") != 0 )
	{
		printf("\e[?1049h");
		fflush(stdout);
	}
#endif
}
