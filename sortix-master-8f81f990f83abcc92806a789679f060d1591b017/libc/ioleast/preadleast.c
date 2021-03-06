/*
 * Copyright (c) 2012, 2013, 2015 Jonas 'Sortie' Termansen.
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
 * ioleast/preadleast.c
 * Reads at least as much data as requested or more from the given offset.
 */

#include <assert.h>
#include <errno.h>
#include <ioleast.h>
#include <stdint.h>
#include <unistd.h>

size_t preadleast(int fd, void* buf_ptr, size_t least, size_t max, off_t off)
{
	assert(least <= max);
	unsigned char* buf = (unsigned char*) buf_ptr;
	size_t done = 0;
	do
	{
		ssize_t amount = pread(fd, buf + done, max - done, off + done);
		if ( amount < 0 )
			return done;
		if ( !amount && done < least )
			return errno = EEOF, done;
		if ( !amount )
			break;
		done += amount;
	} while ( done < least );
	return done;
}
