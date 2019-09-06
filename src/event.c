#include <stdio.h>
#include <stdint.h>

#include "error.h"
#include "ipc.h"
#include "event.h"


error_code event_send(enum event_type evt, uint8_t *data, uint8_t data_len)
{
	ipc_send_event(evt, data, data_len);

	return SUCCESS;
}
