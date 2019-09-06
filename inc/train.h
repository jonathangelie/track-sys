/*
 *  Copyright (C) 2019  Jonathan Gelie <contact@jonathangelie.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TRAIN_HEADER
#define TRAIN_HEADER

#define TRAIN_NAME_LENGTH	20

enum train_state {
	state_moving = 0,
	state_stopped,
};

struct train {
	uint8_t name[TRAIN_NAME_LENGTH];
	uint8_t name_len;
	struct track *on_track;
	enum side dir;
	enum train_state state;
	struct train *next;
};

error_code train_add(uint8_t *name, uint8_t name_len, uint8_t *track_name,
		uint8_t track_name_len, enum side dir);

error_code train_move_to(const uint8_t *name, struct track* tck);

error_code train_set_state(const uint8_t *name, enum train_state state);

uint8_t train_count(void);

const struct train* train_get_list(void);

void train_print_list(void);

/**
 * @brief clearing all trains defined
 */
void train_clear_all(void);

#endif /* TRAIN_HEADER */
