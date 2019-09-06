#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"
#include "error.h"
#include "track.h"
#include "train.h"
#include "mem.h"
#include "dbg.h"

static struct {
	struct train *head;
	uint8_t cnt;
} train_ctx = {};


static struct train* train_new(uint8_t *name, uint8_t name_len)
{
	struct train* t;

	if (SUCCESS != mem_new((void *)&t, sizeof(*t))) {
		return NULL;
	}

	train_ctx.cnt++;

	t->name_len = MIN(TRAIN_NAME_LENGTH, name_len);
	memcpy(t->name, name, t->name_len);

	return t;
}

void train_clear_all(void)
{
	struct train* current = train_ctx.head;
	struct train* next;

	while (current != NULL)
	{
	    next = current->next;
	    mem_free(current);
	    current = next;
	}

	mem_free(current);
	train_ctx.head = NULL;
}

error_code train_add(uint8_t *name, uint8_t name_len, uint8_t *track_name,
		uint8_t track_name_len, enum side dir)
{
	if (sides <= dir) {
		return ERROR_PARAMS;
	}
	struct track * initial_track;

	initial_track = track_get_by_name(track_name);
	if (!initial_track) {
		printf("track not found\n");
		return ERROR_NOT_FOUND;
	}

	static struct train *new_train;

	new_train = train_new(name, name_len);
	if (!new_train) {
		return ERROR_MEM;
	}

	new_train->dir = dir;
	new_train->state = state_moving;
	new_train->on_track = initial_track;

	if (!train_ctx.head) {
		train_ctx.head = new_train;
		train_ctx.head->next = NULL;
		return SUCCESS;
	}

	struct train *current;
	current = train_ctx.head;

	while (current->next != NULL) {
		current = current->next;
	}
	current->next = new_train;

	return SUCCESS;
}

static struct train * train_get_by_name(const uint8_t *name)
{
	struct train *t;

	if (!train_ctx.head)
		return NULL;

	t = train_ctx.head;

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

error_code train_set_state(const uint8_t *name, enum train_state state)
{
	struct train *t;

	t = train_get_by_name(name);
	if (!t)
		return ERROR_PARAMS;

	t->state = state;

	return SUCCESS;
}

error_code train_move_to(const uint8_t *name, struct track* tck)
{
	struct train *t;

	t = train_get_by_name(name);
	if (!t)
		return ERROR_PARAMS;

	t->on_track = tck;

	return SUCCESS;
}

static void train_print(uint8_t idx, struct train *t)
{
	DBG_INFO("#%d- name:%s\n"
			"name_len:%d\n"
			"on track: %s\n"
			"direction: %s\n",
			idx, t->name, t->name_len,
			t->on_track->name,
			(t->dir == est) ? "eastbound" : "westbound");
}

void train_print_list(void)
{
	if (!train_ctx.head) {
		DBG_INFO("no train\n");
		return;
	}
	struct train *current;
	uint8_t idx = 0;

	current = train_ctx.head;

	while (current->next != NULL) {
		train_print(idx, current);
		idx++;
		current = current->next;
	}
	train_print(idx, current);
}

uint8_t train_count(void)
{
	return train_ctx.cnt;
}

const struct train* train_get_list(void)
{
	return train_ctx.head;
}
