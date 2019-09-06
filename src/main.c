#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>


#include "error.h"
#include "cmd.h"
#include "dbg.h"

#include "simulation.h"

void print_usage() {
    printf("Usage: track-helper [--with_ui]\n");
}

int main(int argc, char *argv[])
{
    int opt= 0;
    int long_index = 0;
    bool ui = false;

	static struct option long_options[] = {
		{ "with_ui", no_argument, 0, 'u' },
		{ 0, 0, 0, 0 }
	};


	while ((opt = getopt_long(argc, argv, "u", long_options, &long_index)) != -1) {
		switch (opt) {
		case 'u':
			printf("UI\n");
			ui = true;
			break;
		default:
			print_usage();
			exit(EXIT_FAILURE);
		}
	}
	printf("TRACK SYSTEM\n");

	cmd_init(ui);
	dbg_init(ui);

	while(1) {
		cmd_read();
		simulation_refresh();
	}
	return SUCCESS;

}
