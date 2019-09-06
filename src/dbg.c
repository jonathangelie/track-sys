#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "error.h"
#include "ipc.h"

static bool _withUi;

void dbg_init(bool withUi)
{
	_withUi = withUi;
}

void dbg_print(const char * fmt, ...)
{
	static uint8_t data [80] = {};
	uint16_t data_len;
	va_list p_args;

	memset(data, '\0', sizeof(data));
	va_start(p_args, fmt);
	data_len = vsnprintf((char *)data, sizeof(data)-1, fmt, p_args);
	va_end(p_args);

	if (_withUi) {
		printf("%s\n", data);
		ipc_send_dbg(data, data_len);
	} else
		printf("%s\n", data);
}
