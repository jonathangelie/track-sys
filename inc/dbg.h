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
#ifndef DBG_HEADER
#define DBG_HEADER

void dbg_init(bool withUi);
void dbg_print(const char * fmt, ...);

#define DBG_ERR(fmt, ...) { dbg_print("[ERROR] "fmt, ##__VA_ARGS__);}
#define DBG_INFO(fmt, ...) { dbg_print(fmt, ##__VA_ARGS__);}

#endif /* DBG_HEADER */
