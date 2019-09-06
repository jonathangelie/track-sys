#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>

#include <mqueue.h>

#include "error.h"
#include "mq.h"
#include "dbg.h"

#define MQUEUE_MODE		(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define MQUEUE_KEY_RCV	"/cmd"
#define MQUEUE_KEY_SEND	"/resp"
#define MQUEUE_MSG_TYPE	1
#define MQUEUE_TIMEOUT	1000

enum mqueue_direction {
	receiving = 0,
	sending,
	direction_cnt,
};

struct {
	int msgid[direction_cnt];
	mqueue_rcv_cb cb;
} mqueue_ctx;

error_code mqueue_init(mqueue_rcv_cb cb)
{

	if (!cb)
		return ERROR_PARAMS;

	mqueue_ctx.cb = cb;

	struct mq_attr attr;

	/* Form the queue attributes */
	attr.mq_flags = 0; /* i.e mq_send will be block if message queue is full */
	attr.mq_maxmsg = 4;
	attr.mq_msgsize = sizeof(struct mqueue_msg);
	attr.mq_curmsgs = 0;

	/* remove message queue from the system*/
	mq_unlink(MQUEUE_KEY_RCV);
	mq_unlink(MQUEUE_KEY_SEND);

	mqueue_ctx.msgid[receiving] = mq_open(MQUEUE_KEY_RCV, O_RDWR | O_CREAT,
			MQUEUE_MODE, &attr);
    if (mqueue_ctx.msgid[receiving] < 0) {
    	printf("error creating receiving mqueue\n");
    	perror("mq_open");
    	return ERROR_IPC;
    }

	mqueue_ctx.msgid[sending] = mq_open(MQUEUE_KEY_SEND, O_RDWR | O_CREAT,
			MQUEUE_MODE, &attr);
    if (mqueue_ctx.msgid[sending] < 0) {
    	printf("error creating sending mqueue\n");
    	perror("mq_open");
    	return ERROR_IPC;
    }

    printf("messages queues created [OK] receive = %d send = %d\n",
    		mqueue_ctx.msgid[receiving], mqueue_ctx.msgid[sending]);

    return SUCCESS;
}

void mqueue_close(void)
{
	mq_close(mqueue_ctx.msgid[receiving]);
	mq_close(mqueue_ctx.msgid[sending]);
}

error_code mqueue_send(struct mqueue_msg *msg)
{
	if (mq_send(mqueue_ctx.msgid[sending], (char *)msg, sizeof(msg->data), 0) < 0) {
    	printf("[ERROR] ipc send\n");
		return ERROR_IPC;
	}
	printf("[OK] ipc send\n");
    return SUCCESS;
}

error_code mqueue_receive(void)
{
	ssize_t data_len;
	struct mqueue_msg msg;

	struct timespec ts;
	struct timeval tv;
	int sec = 0;

	// get the current time
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000;

	// compute the timeout
	sec = MQUEUE_TIMEOUT / 1000;

	// perform the addition
	ts.tv_nsec += (MQUEUE_TIMEOUT - sec * 1000) * 1000000;

	// adjust the time
	ts.tv_sec += ts.tv_nsec / 1000000000 + sec;
	ts.tv_nsec = ts.tv_nsec % 1000000000;

	memset(&msg, 0, sizeof(msg));

	data_len = mq_timedreceive(mqueue_ctx.msgid[receiving], (char*) &msg,
			sizeof(msg.data), NULL, &ts);
	if (data_len > 0) {
		printf("mqueue receive (%d)%s\n", (uint8_t)data_len, &msg.data[0]);
		if (mqueue_ctx.cb)
			mqueue_ctx.cb(&msg.data[0], data_len);
		return SUCCESS;
	}
	//DBG_INFO("nothing received\n");

	return ERROR_NOT_FOUND;

}
