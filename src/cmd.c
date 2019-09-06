#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "error.h"
#include "cmd.h"
#include "track.h"
#include "connection.h"
#include "train.h"
#include "simulation.h"
#include "utils.h"
#include "ipc.h"
#include "mem.h"

#define CMD_LINE_LENGTH	80 /*<! command line could not exceed this length */


static error_code cmd_help(uint8_t *data, uint8_t data_len);
static error_code cmd_track_add(uint8_t *data, uint8_t data_len);
static error_code cmd_track_list(uint8_t *data, uint8_t data_len);
static error_code cmd_continuation_add(uint8_t *data, uint8_t data_len);
static error_code cmd_junction_add(uint8_t *data, uint8_t data_len);
static error_code cmd_train_add(uint8_t *data, uint8_t data_len);
static error_code cmd_signal_add(uint8_t *data, uint8_t data_len);
static error_code cmd_write(uint8_t *data, uint8_t data_len);
static error_code cmd_clear(uint8_t *data, uint8_t data_len);
static error_code cmd_simualtion_start(uint8_t *data, uint8_t data_len);
static error_code cmd_simualtion_stop(uint8_t *data, uint8_t data_len);
static error_code cmd_mem_info(uint8_t *data, uint8_t data_len);
static error_code cmd_quit(uint8_t *data, uint8_t data_len);

static const struct {
	const char *name;
	error_code (*func) (uint8_t *data, uint8_t data_len);
	const char *params;
	const char *desc;
} cmd_list[] = {
	{"help", cmd_help,   "",
			"Show this help"},
	{"track-add", cmd_track_add,   "name",
			"adding track"},
	{"track-list", cmd_track_list,   "",
			"listing existing track"},
	{"continuation-add", cmd_continuation_add, "track1 [west|est] track2 [west|est]",
			"adding continuation between 2 tracks"},
	{"junction-add", cmd_junction_add, "track1 [west|est] track2 [west|est]",
			"adding junction between 2 tracks"},
	{"train-add", cmd_train_add, "train_name track_name [westbound|estbound]",
			"adding junction between 2 tracks"},
	{"signal-add", cmd_signal_add, "track_name [westbound|estbound]",
			"adding junction between 2 tracks"},
	{"write", cmd_write,   "[file_location]",
			"Writing track map to file"},
	{"clear-all", cmd_clear,   "",
			"clearing current train system"},
	{"exit", cmd_quit,   "",
			"Exit"},
	{"simulation-start", cmd_simualtion_start,   "",
			"start train system simulation"},
	{"simulation-stop", cmd_simualtion_stop,   "",
			"stop train system simulation"},
	{"mem-info", cmd_mem_info,   "",
			"memory status"},
};

static bool _withUi;

static error_code cmd_help(uint8_t *data, uint8_t data_len)
{
	uint8_t idx;

	for (idx = 0; idx < ARRAY_SIZE(cmd_list); idx++)
		printf("#%-15s %-30s %s\n", cmd_list[idx].name,
				cmd_list[idx].params, cmd_list[idx].desc);

	return SUCCESS;
}

static error_code cmd_track_add(uint8_t *data, uint8_t data_len)
{
	printf("%s %s(%d)\n", __FUNCTION__, data, data_len);
	error_code ret;

	if (!data_len || data[0] == '\0') {
		cmd_help(NULL, 0);
		return ERROR_PARAMS;
	}

	ret = track_add(data, data_len);

	ipc_send_rsp(ret, NULL, 0);

	return ret;
}

struct cmd_connection_info {
	uint8_t *name;
	enum side pos;
} infos[2];

static error_code cmd_connection_parse(uint8_t *data, uint8_t data_len,
		struct cmd_connection_info *conn_infos, uint8_t info_cnt)
{
	if (!data_len || data[0] == '\0') {
		cmd_help(NULL, 0);
		return ERROR_PARAMS;
	}

	uint8_t *param;
	uint8_t idx;

	param = &data[0];
	for (idx = 0; idx < info_cnt; idx++) {

		printf("%d param %s-%ld\n", idx, param, strlen((const char *)param));

		if (*param == '\0') {
			printf("please specify name\n");
			return ERROR_PARAMS;
		}

		conn_infos[idx].name = param;
		param = (uint8_t *)strchr((const char *)param, '\0');
		if (!param || (&data[data_len] <= param)) {
			printf("%d please specify position\n", idx);
			return ERROR_PARAMS;
		}
		param++;

		printf("%d param %s-%ld\n", idx, param, strlen((const char *)param));
		if (memcmp(param, "est", 3) && memcmp(param, "west", 4)) {
			printf("%d please choose between est and west\n", idx);
			return ERROR_PARAMS;
		}

		conn_infos[idx].pos = (!memcmp(param, "west", 4)) ?
				west : est;
		param = (uint8_t *)strchr((const char *)param, '\0');
		if (!param) {
			return ERROR_PARAMS;
		}
		param++;
	}

	return SUCCESS;
}

static error_code cmd_continuation_add(uint8_t *data, uint8_t data_len)
{
	printf("%s\n", __FUNCTION__);

	struct	cmd_connection_info infos[2];
	error_code ret;

	ret = cmd_connection_parse(data, data_len, infos, 2);
	if (SUCCESS != ret)
		return ret;

	ret =  connection_add(conn_continuation, infos[0].name,
			infos[0].pos, infos[1].name, infos[1].pos);

	ipc_send_rsp(ret, NULL, 0);

	return ret;
}


static error_code cmd_junction_add(uint8_t *data, uint8_t data_len)
{
	struct	cmd_connection_info infos[2];
	error_code ret;

	ret = cmd_connection_parse(data, data_len, infos, 2);
	if (SUCCESS != ret)
		return ret;

	ret =  connection_add(conn_junction, infos[0].name,
			infos[0].pos, infos[1].name, infos[1].pos);

	ipc_send_rsp(ret, NULL, 0);

	return ret;

}

static error_code cmd_train_add(uint8_t *data, uint8_t data_len)
{
	uint8_t *name;
	uint8_t name_len;
	uint8_t *track_name;
	uint8_t track_name_len;
	enum side dir;
	uint8_t *param;
	error_code ret;

	if (!data_len || data[0] == '\0') {
		cmd_help(NULL, 0);
		ipc_send_rsp(ERROR_PARAMS, NULL, 0);
		return ERROR_PARAMS;
	}

	name = &data[0];
	name_len = strlen((const char *)name);
	printf("train name [%s]\n", name);

	param = (uint8_t *)strchr((const char *)&data[0], '\0');
	if (!param || (&data[data_len] <= param)) {
		printf("please specify track name\n");
		ipc_send_rsp(ERROR_PARAMS, NULL, 0);
		return ERROR_PARAMS;
	}
	param++;
	track_name = param;
	track_name_len = strlen((const char *)track_name);
	printf("track name [%s]\n", track_name);

	param = (uint8_t *)strchr((const char *)param, '\0');
	if (!param) {
		printf("please specify train direction\n");
		ipc_send_rsp(ERROR_PARAMS, NULL, 0);
		return ERROR_PARAMS;
	}
	param++;

	if (memcmp(param, "estbound", 8) && memcmp(param, "westbound", 9)) {
		printf("please choose between estbound and westbound\n");
		printf("[%s]\n", param);
		ipc_send_rsp(ERROR_PARAMS, NULL, 0);
		return ERROR_PARAMS;
	}

	dir = (!memcmp(param, "estbound", 8)) ? est : west;

	ret =  train_add(name, name_len, track_name, track_name_len, dir);


	ipc_send_rsp(ret, NULL, 0);

	return ret;
}

static error_code cmd_signal_add(uint8_t *data, uint8_t data_len)
{
	uint8_t *track_name;
	uint8_t track_name_len;
	enum side dir;
	uint8_t *param;
	error_code ret;

	if (!data_len || data[0] == '\0') {
		cmd_help(NULL, 0);
		ipc_send_rsp(ERROR_PARAMS, NULL, 0);
		return ERROR_PARAMS;
	}

	track_name = &data[0];
	track_name_len = strlen((const char *)track_name);
	printf("track name [%s]\n", track_name);

	param = (uint8_t *)strchr((const char *)&data[0], '\0');
	if (!param || (&data[data_len] <= param)) {
		printf("please specify signal position\n");
		ipc_send_rsp(ERROR_PARAMS, NULL, 0);
		return ERROR_PARAMS;
	}
	param++;

	if (memcmp(param, "est", 8) && memcmp(param, "west", 9)) {
		printf("please choose between est and west\n");
		printf("[%s]\n", param);
		ipc_send_rsp(ERROR_PARAMS, NULL, 0);
		return ERROR_PARAMS;
	}

	dir = (!memcmp(param, "est", 8)) ? est : west;

	ret =  signal_add(track_name, track_name_len, dir);

	ipc_send_rsp(ret, NULL, 0);

	return ret;
}

static error_code cmd_track_list(uint8_t *data, uint8_t data_len)
{
	printf("%s\n", __FUNCTION__);

	//track_print_list();
	uint8_t cnt;

	cnt = track_count();
	uint8_t buf[2];

	sprintf((char *)buf, "%d", cnt);
	ipc_send_rsp(SUCCESS, (const uint8_t * )buf, 1);
	if (cnt) {

		const struct track *current = track_get_list();

		while (current->next != NULL) {
			ipc_send_rsp(SUCCESS, current->name, current->name_len);
			current = current->next;
		}
		ipc_send_rsp(SUCCESS, current->name, current->name_len);
	}
	return SUCCESS;
}

static error_code cmd_write(uint8_t *data, uint8_t data_len)
{
	printf("%s\n", __FUNCTION__);

	ipc_send_rsp(ERROR_NOT_IMPLEMENTED, NULL, 0);

	return ERROR_NOT_IMPLEMENTED;
}

static error_code cmd_mem_info(uint8_t *data, uint8_t data_len)
{
	printf("%s\n", __FUNCTION__);

	mem_info();

	return SUCCESS;
}

static error_code cmd_clear(uint8_t *data, uint8_t data_len)
{
	printf("%s\n", __FUNCTION__);

	track_clear_all();
	train_clear_all();

	ipc_send_rsp(SUCCESS, NULL, 0);

	return SUCCESS;
}

static error_code cmd_simualtion_start(uint8_t *data, uint8_t data_len)
{
	error_code ret;

	printf("%s\n", __FUNCTION__);

	ret = simulation_start();

	ipc_send_rsp(ret, NULL, 0);

	return ret;
}

static error_code cmd_simualtion_stop(uint8_t *data, uint8_t data_len)
{
	error_code ret;

	printf("%s\n", __FUNCTION__);

	ret = simulation_stop();

	ipc_send_rsp(ret, NULL, 0);

	return SUCCESS;
}

error_code cmd_quit(uint8_t *data, uint8_t data_len)
{
	/* sending response first */
	ipc_send_rsp(SUCCESS, NULL, 0);

	simulation_stop();
	track_clear_all();
	train_clear_all();
	ipc_close();

	exit(0);
	return SUCCESS;
}

error_code cmd_execute(uint8_t *line, uint8_t len)
{
    int i;

    if (*line == '\0')
        return ERROR_PARAMS;

    /* replacing space by end of string */
    for (i = 0; i < len; i++) {
    	if (line[i] == ' ') {

    		line[i] = '\0';
    	}
    }
    line[i] = '\0';

    for (i = 0; i < ARRAY_SIZE(cmd_list); i++) {
        if (!strcasecmp(cmd_list[i].name, (const char *)line)) {
        	/* command exist */
			if (cmd_list[i].name) {

				/* get parameters */
				uint8_t *offset;

				offset = (uint8_t *)strchr((const char *)line, '\0');
				if (&line[len] == offset) {
					/* no parameter */
					printf("no params\n");
					len = 0;
					offset = NULL;
				} else {
					offset++;
					len -= offset - line;
					printf("new len %d\n", len);
				}
				return cmd_list[i].func((uint8_t *)offset, len);
			}
        }

    }

    cmd_help(NULL, 0);
    ipc_send_rsp(ERROR_NOT_FOUND, NULL, 0);

    return ERROR_NOT_FOUND;
}
error_code cmd_read_from_ipc(void)
{

	ipc_read();

	return SUCCESS;
}

error_code cmd_read_from_stdin(void)
{
	size_t len = 0; /* Current offset of the line */
	char line[CMD_LINE_LENGTH];
	int c;

	/* Read char by char, breaking if we reach EOF or a newline */
	while ((c = fgetc(stdin)) != '\n' && !feof(stdin)) {
		line[len] = c;

		if (++len >= CMD_LINE_LENGTH) {
			return ERROR_PARAMS;
		}
	}

	line[len] = '\0';

	return cmd_execute((uint8_t *)line, len);
}

error_code cmd_init(bool withUi)
{
	_withUi = withUi;

	if (_withUi)
		return ipc_init();

	return SUCCESS;
}

error_code cmd_read(void)
{
	error_code ret;

	if (_withUi)
		ret = cmd_read_from_ipc();
	else
		ret = cmd_read_from_stdin();

	return ret;
}
