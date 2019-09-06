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
#ifndef IPC_HEADER
#define IPC_HEADER


/**
 * @brief IPC initialization
 *
 * @return 0 for success, otherwise please check @ref error_code
 */

error_code ipc_init(void);
void ipc_close(void);

void ipc_send_dbg(const uint8_t *data, uint8_t data_len);
void ipc_send_rsp(uint8_t status, const uint8_t *data, uint8_t data_len);
void ipc_send_event(uint8_t evt_tpe, const uint8_t *data, uint8_t data_len);

void ipc_read(void);

#endif /* IPC_HEADER */
