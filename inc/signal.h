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
#ifndef SIGNAL_HEADER
#define SIGNAL_HEADER

enum signal_state {
	state_green  = 0,
	state_red,
	state_unknonw,
};

struct signal {
	enum signal_state state;
	uint8_t duration; /*!< signal red light duration */
};

error_code signal_add(uint8_t *track_name, uint8_t track_name_len, enum side pos);
error_code signal_free(struct signal* s);
error_code signal_set_state(struct signal* s, enum signal_state state);
#endif /* SIGNAL_HEADER */
