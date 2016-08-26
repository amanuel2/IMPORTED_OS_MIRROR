/*
 * Copyright (c) 2013, 2015, 2016 Jonas 'Sortie' Termansen.
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
 * find.c
 * Locate files and directories.
 */

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TYPE_BLK (1 << 0)
#define TYPE_CHR (1 << 1)
#define TYPE_DIR (1 << 2)
#define TYPE_FIFO (1 << 3)
#define TYPE_LNK (1 << 4)
#define TYPE_REG (1 << 5)
#define TYPE_SOCK (1 << 6)
#define TYPE_OTHER (1 << 7)
#define ALL_TYPES (TYPE_BLK | TYPE_CHR | TYPE_DIR | TYPE_FIFO | \
                   TYPE_LNK | TYPE_REG | TYPE_SOCK | TYPE_OTHER)

char* AddElemToPath(const char* path, const char* elem)
{
	size_t pathlen = strlen(path);
	size_t elemlen = strlen(elem);
	if ( pathlen && path[pathlen-1] == '/' )
	{
		char* ret = (char*) malloc(sizeof(char) * (pathlen + elemlen + 1));
		stpcpy(stpcpy(ret, path), elem);
		return ret;
	}
	char* ret = (char*) malloc(sizeof(char) * (pathlen + 1 + elemlen + 1));
	stpcpy(stpcpy(stpcpy(ret, path), "/"), elem);
	return ret;
}

bool find(int dirfd, const char* relpath, const char* path, int types)
{
	bool ret = true;
	int fd = openat(dirfd, relpath, O_RDONLY | O_SYMLINK_NOFOLLOW);
	if ( fd < 0 )
	{
		error(0, errno, "%s", path);
		return false;
	}
	struct stat st;
	if ( fstat(fd, &st) )
	{
		error(0, errno, "stat: %s", path);
		return false;
	}
	if ( S_ISBLK(st.st_mode) )
	{
		if ( types & TYPE_BLK )
			printf("%s\n", path);
	}
	else if ( S_ISCHR(st.st_mode) )
	{
		if ( types & TYPE_CHR )
			printf("%s\n", path);
	}
	else if ( S_ISDIR(st.st_mode) )
	{
		if ( types & TYPE_DIR )
			printf("%s\n", path);
		DIR* dir = fdopendir(fd);
		if ( !dir )
		{
			error(0, errno, "fdopendir");
			close(fd);
			return false;
		}
		struct dirent* entry;
		while ( (entry = readdir(dir)) )
		{
			const char* name = entry->d_name;
			if ( !strcmp(name, ".") || !strcmp(name, "..") )
				continue;
			char* newpath = AddElemToPath(path, name);
			if ( !find(fd, name, newpath, types) )
			{
				ret = false;
				break;
			}
		}
		closedir(dir);

	}
	else if ( S_ISFIFO(st.st_mode) )
	{
		if ( types & TYPE_FIFO )
			printf("%s\n", path);
	}
	else if ( S_ISLNK(st.st_mode) )
	{
		if ( types & TYPE_LNK )
			printf("%s\n", path);
	}
	else if ( S_ISREG(st.st_mode) )
	{
		if ( types & TYPE_REG )
			printf("%s\n", path);
	}
	else if ( S_ISSOCK(st.st_mode) )
	{
		if ( types & TYPE_SOCK )
			printf("%s\n", path);
	}
	else
	{
		if ( types & TYPE_OTHER )
			printf("%s\n", path);
	}
	if ( !S_ISDIR(st.st_mode) )
		close(fd);
	return ret;
}

int main(int argc, char* argv[])
{
	const char* path = NULL;
	bool found_options = false;
	int types = 0;
	for ( int i = 1; i < argc; i++ )
	{
		const char* arg = argv[i];
		if ( arg[0] != '-' )
		{
			if ( found_options )
				error(1, 0, "path `%s' must come before options", arg);
			if ( path )
				error(1, 0, "multiple paths are not supported");
			path = arg;
			continue;
		}
		found_options = true;
		if ( !strcmp(arg, "-type") )
		{
			if ( i + 1 == argc )
				error(1, 0, "-type expects an argument");
			arg = argv[++i];
			if ( !strcmp(arg, "b") )
				types |= TYPE_BLK;
			else if ( !strcmp(arg, "c") )
				types |= TYPE_CHR;
			else if ( !strcmp(arg, "d") )
				types |= TYPE_DIR;
			else if ( !strcmp(arg, "f") )
				types |= TYPE_REG;
			else if ( !strcmp(arg, "l") )
				types |= TYPE_LNK;
			else if ( !strcmp(arg, "p") )
				types |= TYPE_FIFO;
			else if ( !strcmp(arg, "S") )
				types |= TYPE_SOCK;
			else
				error(1, 0, "unknown `-type %s'", arg);
		}
		else
			error(1, 0, "unknown option `%s'", arg);
	}
	if ( !path )
		path = ".";
	if ( !types )
		types = ALL_TYPES;
	return find(AT_FDCWD, path, path, types) ? 0 : 1;
}
