/*
 * Copyright (c) 2014 Jonas 'Sortie' Termansen.
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
 * wchar/wcscasecmp.c
 * Compares two strings ignoring case.
 */

#include <stdbool.h>
#include <wchar.h>
#include <wctype.h>

int wcscasecmp(const wchar_t* a, const wchar_t* b)
{
	for ( size_t i = 0; true; i++ )
	{
		wint_t ac = towlower((wint_t) a[i]);
		wint_t bc = towlower((wint_t) b[i]);
		if ( ac == L'\0' && bc == L'\0' )
			return 0;
		if ( ac < bc )
			return -1;
		if ( ac > bc )
			return 1;
	}
}