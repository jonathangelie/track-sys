#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "error.h"
#include "cmd.h"
#include "track.h"
#include "signal.h"
#include "connection.h"
#include "mem.h"

static error_code signal_new(uint8_t *track_name, enum side pos)
{

	struct track *t;
	error_code ret;
	t = track_get_by_name(track_name);
	if (!t) {
		return ERROR_NOT_FOUND;
	}

	struct signal* sign;

	ret = mem_new((void *)&sign, sizeof(*sign));
	if (SUCCESS != ret) {
		return ret;
	}

	sign->state = state_green;
	t->sign[pos] = sign;

	return SUCCESS;
}

error_code signal_add(uint8_t *track_name, uint8_t track_name_len, enum side pos)
{
	return signal_new(track_name, pos);
}

error_code signal_free(struct signal* s)
{
	return mem_free(s);
}

error_code signal_set_state(struct signal* s, enum signal_state state)
{
	if (!s)
		return ERROR_PARAMS;

	s->state = state;

	return SUCCESS;
}
