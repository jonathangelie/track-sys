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
#ifndef MQUEUE_HEADER
#define MQUEUE_HEADER

#define MQUEUE_MSG_LENGTH	100

struct mqueue_msg {
	uint8_t data[MQUEUE_MSG_LENGTH];
};

typedef void (*mqueue_rcv_cb)(uint8_t *data, uint8_t data_len);

error_code mqueue_send(struct mqueue_msg *msg);

error_code mqueue_init(mqueue_rcv_cb cb);

void mqueue_close(void);

error_code mqueue_receive(void);

#endif /* MQUEUE_HEADER */
