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
#ifndef ERROR_HEADER
#define ERROR_HEADER

/**
 * @brief error code
 */
typedef enum  {
	SUCCESS = 0, 	/*!< The operation completed successfully */
	ERROR_ALREADY, 	/*!< Already initialized */
	ERROR_PARAMS,	/*!< The parameter contains invalid data */
	ERROR_NOT_FOUND,/*!< Not found */
	ERROR_WRITE,	/*!< Writing to persistent storage failed */
	ERROR_READ,		/*!< Reading from persistent storage failed */
	ERROR_MEM,		/*!< Insufficient memory */
	ERROR_IPC,		/*!< Interprocess communication error */
	ERROR_WRONG_STATE, /*!<  Wrong state */
	ERROR_NOT_IMPLEMENTED, /*!< Feature not yet implemented */
} error_code;


#endif /* ERROR_HEADER */
