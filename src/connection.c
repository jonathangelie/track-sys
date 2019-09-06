#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "utils.h"
#include "error.h"
#include "cmd.h"
#include "track.h"
#include "signal.h"
#include "connection.h"

static error_code connection_get_junction_idx(uint8_t *idx, struct track *t, enum side s)
{
	*idx = 0;

	if (t->conn[s].type == conn_continuation) {
		return ERROR_ALREADY;
	}

	if (t->conn[s].type == conn_none) {
		return SUCCESS;
	}

	if (t->conn[s].type == conn_junction) {
		for (*idx = 0; *idx < 2 ; idx++) {
			if (!t->conn[s].next.junction[*idx])
				return SUCCESS;
		}

	}

	return ERROR_ALREADY;
}

error_code connection_add(enum connection_type type, uint8_t *name1,
		enum side side1, uint8_t *name2, enum side side2)
{

	struct track *t1;
	struct track *t2;

	if (sides <= side1 && sides <= side2) {
		printf("undefined position\n");
		return ERROR_PARAMS;
	}

	if (side1 == side2) {
		printf("similar position\n");
		return ERROR_PARAMS;
	}

	if (conn_unknown <= type || conn_none >= type) {
		return ERROR_PARAMS;
	}

	t1 = track_get_by_name(name1);
	t2 = track_get_by_name(name2);
	if (!t1 || !t2) {
		printf("track not found\n");
		return ERROR_NOT_FOUND;
	}

	if (conn_continuation == type) {

		if (t1->conn[side1].type != conn_none || t2->conn[side2].type != conn_none) {
			printf("already in used\n");
			return ERROR_ALREADY;
		}
		t1->conn[side1].next.continuation = t2;
		t2->conn[side2].next.continuation = t1;

	} else if (conn_junction == type) {

		uint8_t idx1, idx2;

		if (SUCCESS != connection_get_junction_idx(&idx1, t1, side1))
			return ERROR_ALREADY;

		if (SUCCESS != connection_get_junction_idx(&idx2, t2, side2))
			return ERROR_ALREADY;

		t1->conn[side1].next.junction[idx1] = t2;
		t1->conn[side2].next.junction[idx2] = t1;
	}


	t1->conn[side1].type = type;
	t2->conn[side2].type = type;

	return SUCCESS;
}
