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
#ifndef TRACK_HEADER
#define TRACK_HEADER

#include "utils.h"
#include "connection.h"
#include "signal.h"

#define TRACK_NAME_LENGTH	20
struct track {
	uint8_t name_len;		/*!< track friendly name */
	uint8_t name[TRACK_NAME_LENGTH];	/*!< track friendly name length */
	struct connection conn[sides]; /*!< track connection properties */
	struct signal *sign[sides]; /*!< track connection properties */
	struct track *next; 	/*!< list of track - do not modify */
};

/**
 * @brief adding a new track
 * @param name track name
 * @param name_len name length
 *
 * @return 0 for success, otherwise please check @ref error_code
 *
 */
error_code track_add(uint8_t *name, uint8_t name_len);
/**
 * @brief retrieving a track by its name
 * @param name track name
 *
 * @return track object for success or NULL if track does not exist
 */
struct track * track_get_by_name(const uint8_t *name);
/**
 * @brief print the list of track registered
 */
void track_print_list(void);
/**
 * @brief clearing all tracks defined
 */
void track_clear_all(void);
/**
 * @brief Retrieve the count of track
 *
 * @return track count
 */
uint8_t track_count(void);

/**
 * @brief Retrieve the list of track
 *
 * @return list of track
 */
const struct track* track_get_list(void);

int track_find_next_junction(struct track *t, enum side s, struct track **tracktJunction);
#endif /* TRACK_HEADER */
