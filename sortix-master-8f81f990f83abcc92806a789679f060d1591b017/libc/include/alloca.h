/*
 * Copyright (c) 2013, 2016 Jonas 'Sortie' Termansen.
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
 * alloca.h
 * Stack-based memory allocation.
 */

#ifndef INCLUDE_ALLOCA_H
#define INCLUDE_ALLOCA_H

#include <sys/cdefs.h>

#include <sys/__/types.h>

#ifndef __size_t_defined
#define __size_t_defined
#define __need_size_t
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Declare a function prototype despite that there really is no alloca function
   in Sortix. The compiler will normally be run with -fbuiltin and simply use
   the compiler builtin as intended. */
void* alloca(size_t);

/* In case the compilation is run with -fno-builtin, we'll call the builtin
   function directly, since there is no alloca function in the first place. You
   can simply undef this if you want the real prototype, which will give a link
   error if -fno-builtin is passed. */
#define alloca(size) __builtin_alloca(size)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
