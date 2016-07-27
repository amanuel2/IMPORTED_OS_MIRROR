#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

#include "ansicode.h"

int ansicode_addc (wchar_t wc, struct ansicode_state* state)
{
	if (state->first_c == '\0')
	{
		if (wc == L'[')
		{
			state->first_c = '[';
			return -2;
		}
		return -1;
	}
	else
	{
		if (wc == L'[')
		{
			return -1;
		}
	}
	if (wc == L'm')
	{
		state->cmd = wc;
		return 0;
	}
	if (state->arg_num == 16)
	{
		return -2;
	}
	if (wc == L';')
	{
		state->arg_num += 1;
		return -2;
	}
	if (L'0' <= wc && wc <= L'9')
	{
		if (state->arg_num == 0)
		{
			state->arg_num = 1;
		}
		state->arg[state->arg_num-1] = state->arg[state->arg_num-1] * 10 +
		                               wc - '0';
		return -2;
	}
	return -1;
}

void ansicode_reset (struct ansicode_state* state)
{
	state->first_c = '\0';
	state->cmd = L'\0';
	state->arg_num = 0;
	for (size_t i = 0; i < 16; i++)
	{
		state->arg[i] = 0;
	}
}
