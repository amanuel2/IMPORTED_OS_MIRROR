/*
 * Copyright (c) 2013, 2014, 2015, 2016 Jonas 'Sortie' Termansen.
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
 * brand.h
 * Macros expanding to the name of the operating system and distribution.
 */

#ifndef INCLUDE_BRAND_H
#define INCLUDE_BRAND_H

/*
 * The name 'Sortix' and the Sortix Logo are reserved for use by the official
 * Sortix project. If you publish modified versions of this operating system,
 * first edit this file to make no use of the 'Sortix' name and the Sortix logo
 * in its definitions, then rebuild the whole operating system and ports to
 * change the branding.
 */

/* The name of the distribution of the operation system. */
#define BRAND_DISTRIBUTION_NAME "Sortix"

/* The website of the distribution. */
#define BRAND_DISTRIBUTION_WEBSITE "https://sortix.org"

/* The name of the operating system. */
#define BRAND_OPERATING_SYSTEM_NAME "Sortix"

/* The name of the kernel. */
#define BRAND_KERNEL_NAME "Sortix"

/* The default hostname. */
#define BRAND_DEFAULT_HOSTNAME "sortix"

/* The tagline of this release. */
#define BRAND_RELEASE_TAGLINE "\"I'd like to add you to my professional Sortix network\""

/* The operating system logo as ascii art. */
#define BRAND_LOGO \
"                                        _  \n" \
"                                       / \\ \n" \
"   /\\    /\\                           /   \\\n" \
"  /  \\  /  \\                          |   |\n" \
" /    \\/    \\                         |   |\n" \
"|  O    O    \\_______________________ /   |\n" \
"|                                         |\n" \
"| \\_______/                               /\n" \
" \\                                       / \n" \
"   ------       ---------------      ---/  \n" \
"        /       \\             /      \\     \n" \
"       /         \\           /        \\    \n" \
"      /           \\         /          \\   \n" \
"     /_____________\\       /____________\\  \n" \
"                                           \n" \

/* The operating system logo for panic screens. */
#define BRAND_LOGO_PANIC \
"                                        _  \n" \
"                                       / \\ \n" \
"   /\\    /\\                           /   \\\n" \
"  /  \\  /  \\                          |   |\n" \
" /    \\/    \\                         |   |\n" \
"|  X    X    \\_______________________ /   |\n" \
"|                                         |\n" \
"| _________                               /\n" \
" \\                                       / \n" \
"   ------       ---------------      ---/  \n" \
"        /       \\             /      \\     \n" \
"       /         \\           /        \\    \n" \
"      /           \\         /          \\   \n" \
"     /_____________\\       /____________\\  \n" \
"                                           \n" \

#endif
