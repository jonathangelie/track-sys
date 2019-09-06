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
#ifndef UTILS_HEADER
#define UTILS_HEADER


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))


#define ARRAY_SIZE(array) \
    (sizeof(array) / sizeof(*array))

#ifndef BIT
#define BIT(n) (1UL << (n))
#endif

enum side {
	west = 0,
	est,
	sides,
};
#endif /* UTILS_HEADER */
