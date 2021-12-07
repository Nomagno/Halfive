/*
Redistributions, modified or unmodified, in whole or in part, must retain
applicable notices of copyright or other legal privilege, these conditions, and
the following license terms and disclaimer.  Subject to these conditions, each
holder of copyright or other legal privileges, author or assembler, and
contributor of this work, henceforth "licensor", hereby grants to any person
who obtains a copy of this work in any form:

1. Permission to reproduce, modify, distribute, publish, sell, sublicense, use,
and/or otherwise deal in the licensed material without restriction.

2. A perpetual, worldwide, non-exclusive, royalty-free, gratis, irrevocable
patent license to make, have made, provide, transfer, import, use, and/or
otherwise deal in the licensed material without restriction, for any and all
patents held by such licensor and necessarily infringed by the form of the work
upon distribution of that licensor's contribution to the work under the terms
of this license.

NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS LICENSE
OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS, ASSEMBLERS, OR HOLDERS OF
COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT, OR OTHERWISE ARISING FROM, OUT
OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE
WORK.*/

#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../core/halfive.h"

#define BUFFERCONST 65000

int termwidth;
int termheight;

void handle_input(int height, int width, int maxchar, int inputarr[], int mode,
		  world *proto)
{

	proto->racers[0].active = 1;

	proto->racers[0].axis1 = inputarr[0];
	proto->racers[0].axis2 = inputarr[1];
	proto->racers[0].axis3 = inputarr[2];
	proto->racers[0].axis4 = inputarr[3];

	proto->racers[0].btn1 = inputarr[4];
	proto->racers[0].btn2 = inputarr[5];
	proto->racers[0].btn3 = inputarr[6];
	proto->racers[0].btn4 = inputarr[7];
	int code = H5Sim(proto, 1);
	printf("FOOBAR\n");
	return;
}

int main(int argc, char **argv)
{

	world proto;
	for (int i = 0; i < RACENUM; i++) {
		proto.racers[i].width = 400;
		proto.racers[i].length = 800;
		proto.racers[i].baseline_speed = 20;
		proto.racers[i].baseline_accel = 40;
		proto.racers[i].active = 0;
	}

	int mode = 1;
	struct winsize ws;

	ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
	termwidth = ws.ws_col;
	termheight = ws.ws_row;

	char output[200] = {0};
	FILE *track = fopen("../../assets/tracks/circle_madness.hwt", "r");
	int initerr = H5Init(track, &proto);

	while (mode != 0) {
		fgets(output, sizeof(output) - 1, stdin);
		char *token;
		_Bool exit = 0;
		int inputs[8] = {0, 0, 0, 0, 0, 0, 0, 0};

		token = strtok(output, ",");
		inputs[0] = strtol(token, NULL, 10);
		if (strcmp("EXIT", token) == 0)
			exit = 1;

		for (int i = 0; i < 8; i++) {
			token = strtok(NULL, ",");
			if (token != NULL) {
				inputs[i] = strtol(token, NULL, 10);
				if ((i > 3) && (inputs[i] > 1))
					inputs[i] = 1;
				if (strcmp("EXIT", token) == 0)
					exit = 1;
			}
		}
		if (exit)
			mode = 0;
		if (mode == 0) {
			printf(
			    "Received signal %i (ABORT)\nPipe content dump: \n",
			    mode);
			for (long unsigned int i = 0; i < sizeof(output); i++) {
				printf("%X", output[i]);
			}
			putchar('\n');
			break;
		} else
			handle_input(termheight, termwidth, BUFFERCONST, inputs,
				     0, &proto);
	}
	return 0;
}
