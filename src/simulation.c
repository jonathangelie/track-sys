#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "error.h"
#include "track.h"
#include "connection.h"
#include "train.h"
#include "simulation.h"
#include "utils.h"
#include "dbg.h"
#include "ui.h"
#include "event.h"

enum simulation_state {
	simul_state_stopped  = 0,
	simul_state_running,
	simul_state_unknown,
};

static enum simulation_state state = simul_state_stopped;

error_code simulation_start(void)
{

	if (!train_count() || !track_count()) {
		DBG_ERR(" %d train | %d tracks");
		return ERROR_WRONG_STATE;
	}

	state = simul_state_running;

	ui_refresh();

	return SUCCESS;
}


error_code simulation_stop(void)
{
	state = simul_state_stopped;
	return SUCCESS;
}

bool simulation_is_running(void)
{
	return (simul_state_running == state) ? true : false;
}

static void simulation_refresh_train(const struct train *t)
{

	static uint8_t stopped_cnt = 0; /* saving the number of trains reached tertmination */

	if (t->on_track->sign[t->dir] &&  state_red == t->on_track->sign[t->dir]->state) {
		DBG_INFO("signal RED for %s on track\n", t->name, t->on_track->name);
		t->on_track->sign[t->dir]->duration--;
		train_set_state(t->name, state_stopped);
		return;
	}

	if (t->state == state_stopped) {
		DBG_INFO("train %s already stopped\n",t->name);
		/* this happens when train already reached a termination */
		return;
	}

	DBG_INFO("train %s on track %s to %d\n",
			t->name, t->on_track->name, t->dir);

	struct connection *conn;

	conn = &t->on_track->conn[t->dir];

	if (conn_none == conn->type) {
		/* train just reached a termination */
		DBG_INFO("train @ %s stopped\n", t->on_track->name);
		train_set_state(t->name, state_stopped);
		stopped_cnt++;
		if (stopped_cnt == train_count()) {
			simulation_stop();
			event_send(event_simulation_stopped, NULL, 0);
		}
		return;
	}

	if (conn_continuation == conn->type) {
		struct track *trackNextJunction, *tmp;
		int step;

		step = track_find_next_junction(t->on_track, t->dir, &trackNextJunction);

		struct train *tt = t->next;
		int s;
		while (tt) {

			/* looking over existing train to see if a collision might occurs at a junction */
			if (tt->dir == t->dir || (tt->state == state_stopped)) {
				/* same direction, no collision OR already stopped */
				tt = tt->next;
				continue;
			}

			s = track_find_next_junction(tt->on_track, tt->dir, &tmp);
			if (tmp->conn[t->dir].next.junction[0] == trackNextJunction ||
				tmp->conn[t->dir].next.junction[1] == trackNextJunction) {
				/* trains will met in MIN(step, s) steps */
				if (trackNextJunction->conn[t->dir].next.junction[0] &&
					trackNextJunction->conn[t->dir].next.junction[1]) {
					/* priority goes for the one with multiple choice,
					 * otherwise tracks might be face to face */

					signal_add(tt->on_track->name, tt->on_track->name_len, tt->dir);
					tt->on_track->sign[tt->dir]->state = state_red;
					tt->on_track->sign[tt->dir]->duration = step;

					train_move_to(t->name, conn->next.continuation);
					return;

				} else {
					signal_add(t->on_track->name, t->on_track->name_len, t->dir);
					t->on_track->sign[t->dir]->state = state_red;
					t->on_track->sign[t->dir]->duration = s;
				}

			}
			tt = tt->next;
		}
		train_move_to(t->name, conn->next.continuation);
		return;
	}

	if (conn_junction == conn->type) {


		struct track *trackNextJunction;
		struct train *tt = t->next;

		while (tt) {

			/* checking a train is going in the opposite direction in one of the forked tracks */

			track_find_next_junction(tt->on_track, tt->dir, &trackNextJunction);

			if (conn->next.junction[0] == trackNextJunction) {
				train_move_to(t->name, conn->next.junction[1]);
				return;
			} else if (conn->next.junction[1] == trackNextJunction) {
				train_move_to(t->name, conn->next.junction[0]);
				return;
			}
			tt = tt->next;
		}
		/* arbitrary goes to first junction element */
		train_move_to(t->name, conn->next.junction[0]);
	}

}

void simulation_refresh(void)
{
	if (!simulation_is_running())
		return;

	const struct train *t;

	t = train_get_list();
	do {
		simulation_refresh_train(t);
		t = t->next;
	} while (t != NULL);

}
