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
#ifndef CONNECTION_HEADER
#define CONNECTION_HEADER


#define CONNECTION_TRACK_BY_JUNCTION	2 /*!< a junction is compound of a maximum of two tracks */

#if 0
/**
 * @brief connection location
 */
enum position {
	position_west = 0,	/*!<  connection on the West side of the track */
	position_est,		/*!<  connection on the Est side of the track */
	position_available,	/*!<  maximum of location available */
};
#endif
/**
 * @brief connection type
 */
enum connection_type {
	conn_none = 0,		/*!< No connection */
	conn_continuation,	/*!< continuation */
	conn_junction,		/*!< junction */
	conn_unknown,		/*!< connection unknown */
};

/** @brief connection object
 *
 */
struct connection {
	enum connection_type type; /*!< connection type */
	union {
		struct track * continuation;
		struct track * junction[CONNECTION_TRACK_BY_JUNCTION];
	} next; /*!< connection is a continuation or junction */
};

/**
 * @brief adding a connection
 * @paramn conn_type connection type
 * @param name1 name of the first track
 * @param pos1 connection location on the first track
 * @param name2 name of the second track
 * @param pos2 connection location on the second track
 *
 * @return 0 for success, otherwise please check @ref error_code
 */
error_code connection_add(enum connection_type type, uint8_t *name1,
	enum side sid1, uint8_t *name2, enum side side2);


#endif /* CONNECTION_HEADER */
