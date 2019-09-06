#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "error.h"
#include "ipc.h"
#include "cmd.h"
#include "mq.h"

enum ipc_msg_type {
	ipc_msg_rsp = 0,
	ipc_msg_event,
	ipc_msg_dbg,
};

struct ipc_msg {
	uint8_t type; /*<! @ef ipc_msg_type */
	uint8_t status;
	uint8_t data_len;
	const uint8_t *data;
};

static void ipc_rcv_cb(uint8_t *data, uint8_t data_len)
{
	cmd_execute(data, data_len);
}

error_code ipc_init(void)
{
	return mqueue_init(ipc_rcv_cb);
}

void ipc_close(void)
{
	mqueue_close();
}

void ipc_read(void)
{
	mqueue_receive();
}

static struct mqueue_msg qmsg;

static void ipc_send_msg(struct ipc_msg *msg)
{

	memset(&qmsg, '\0', sizeof(qmsg));

	uint8_t *data;

	data = &qmsg.data[0];
	data += snprintf((char *)&qmsg.data[0], sizeof(qmsg.data),
			"%02d %02d %02d ",  msg->type, msg->status, msg->data_len);

	if (msg->data_len)
		snprintf((char *)data, &qmsg.data[MQUEUE_MSG_LENGTH -1] - data,
				"%s", msg->data);

	printf("%s\n", (const char *)qmsg.data);

	mqueue_send(&qmsg);
}

static void _ipc_send_rsp(uint8_t type, uint8_t status, const uint8_t *data,
		uint8_t data_len)
{
	struct ipc_msg msg = {
		.type = type,
		.status = status,
		.data = data,
		.data_len = data_len,
	};
	ipc_send_msg(&msg);
}

void ipc_send_dbg(const uint8_t *data, uint8_t data_len)
{
	_ipc_send_rsp(ipc_msg_dbg, SUCCESS, data, data_len);
}

void ipc_send_rsp(uint8_t status, const uint8_t *data, uint8_t data_len)
{
	_ipc_send_rsp(ipc_msg_rsp, status, data, data_len);
}

void ipc_send_event(uint8_t evt_tpe, const uint8_t *data, uint8_t data_len)
{
	_ipc_send_rsp(ipc_msg_event, evt_tpe, data, data_len);
}
