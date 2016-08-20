#ifndef ANSICODE_H
#define ANSICODE_H

#include <stdbool.h>
#include <wchar.h>

struct ansicode_state
{
	char first_c;
	wchar_t cmd;
	size_t arg_num;
	int arg[16];
};

int ansicode_addc (wchar_t, struct ansicode_state*);
void ansicode_reset (struct ansicode_state*);

#endif
