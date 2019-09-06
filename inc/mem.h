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
#ifndef MEM_HEADER
#define MEM_HEADER

error_code _mem_new(void **ptr, size_t len, const char* caller);
/**
 * @brief memory allocation
 * @param ptr pointer to memory allocated
 * @param len memory size to allocate
 *
 * @return 0 for success, otherwise please check @ref error_code
 */
#define mem_new(ptr, len) _mem_new(ptr, len, __FUNCTION__)
/**
 * @brief releasing memory
 * @param ptr pointer to memory to release
 *
 * @return 0 for success, otherwise please check @ref error_code
 */
error_code mem_free(void *ptr);
/**
 * @brief memory status
 */
void mem_info(void);
#endif /* MEM_HEADER */
