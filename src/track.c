#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "error.h"
#include "connection.h"
#include "track.h"
#include "mem.h"

static struct {
	struct track *head;
	uint8_t cnt;
} track_ctx = {};

/* track is dynamically allocated, user must free-up memory */
static struct track* track_new(uint8_t *name, uint8_t name_len)
{

	struct track* t;

	if (SUCCESS != mem_new((void *)&t, sizeof(*t))) {
		return NULL;
	}

	t->name_len = MIN(TRACK_NAME_LENGTH, name_len);
	memcpy(t->name, name, t->name_len);

	return t;
}

void track_clear_all(void)
{
	struct track* current = track_ctx.head;
	struct track* next;

	while (current != NULL)
	{
	    next = current->next;
	    signal_free(current->sign[est]);
	    signal_free(current->sign[west]);
	    mem_free(current);
	    current = next;
	}
    signal_free(current->sign[est]);
    signal_free(current->sign[west]);
	mem_free(current);

	track_ctx.head = NULL;
	track_ctx.cnt = 0;
}

error_code track_add(uint8_t *name, uint8_t name_len)
{
	/* check if track already exist */
	if (track_get_by_name(name)) {
		printf("%s already exist\n", name);
		return ERROR_ALREADY;
	}

	static struct track *new_track;

	new_track = track_new(name, name_len);
	if (!new_track) {
		return ERROR_MEM;
	}

	track_ctx.cnt++;
	if (!track_ctx.head) {
		track_ctx.head = new_track;
		track_ctx.head->next = NULL;
		return SUCCESS;
	}

	struct track *current;
	current = track_ctx.head;

	while (current->next != NULL) {
		current = current->next;
	}
	current->next = new_track;

	return SUCCESS;
}


struct track * track_get_by_name(const uint8_t *name)
{
	struct track *t;

	if (!track_ctx.head)
		return NULL;

	t = track_ctx.head;

	while (t->next != NULL) {
		if (!memcmp(t->name, name, t->name_len)) {
			return t;
		}
		t = t->next;
	}

	if (!memcmp(t->name, name, t->name_len)) {
		return t;
	}

	return NULL;
}

static void track_print(uint8_t idx, struct track *t)
{
	printf("#%d- name:%s\n"
			"name_len:%d\n"
			"connection:\n"
				"\t-west: type:%d\n"
				"\t-est: type:%d\n\n",
				idx, t->name, t->name_len,
				t->conn[west].type, t->conn[est].type);
}

void track_print_list(void)
{
	if (!track_ctx.head) {
		printf("no track\n");
		return;
	}
	struct track *current;
	uint8_t idx = 0;

	current = track_ctx.head;

	while (current->next != NULL) {
		track_print(idx, current);
		idx++;
		current = current->next;
	}
	track_print(idx, current);
}

uint8_t track_count(void)
{
	return track_ctx.cnt;
}

const struct track* track_get_list(void)
{
	return track_ctx.head;
}


int track_find_next_junction(struct track *t, enum side s, struct track **tracktJunction)
{
	int step = -1;
	struct track *current;

	current = t;
	*tracktJunction = NULL;

	while (current->conn[s].type != conn_none) {
		if (current->conn[s].type == conn_junction) {
			*tracktJunction = current;
			break;
		}
		if (current->conn[s].type == conn_continuation)
			current = current->conn[s].next.continuation;
		step++;
	};

	return step;
}

