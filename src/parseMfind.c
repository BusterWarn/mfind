/*
* Parser for mfind.c. Will parse main arguments and read the options, storing
* them in a struct - args.
*
* Author: Buster Hultgren Wärn <dv17bhn@cs.umu.se>
*
* Final build: 2018-10-26
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "parseMfind.h"
#include "saferMemHandler.h"

/*
* description: Main parser for arguments. Uses getopt to parse flags. Rest of
* the arguments are read as starting positions or the target.
* param[in]: a - Pointer to args struct. Arguments will be stored here.
* param[in]: argc - Number of arguments.
* param[in]: argv - The arguments.
*/
void parseArgs (args *a, int argc, char *argv[]) {

	/* While loop reading through flags using getopt*/
	int opt;
	int nrthr = 0;
	while ((opt = getopt(argc, argv, "t:p:")) != -1) {

		switch (opt) {

			case 't':
				if (optarg[0] != 'd' && optarg[0] != 'f' && optarg[0] != 'l') {

					fprintf(stderr, "Invalid argument: -t must be set to d, "
									 "f or l\n");
					argsKill(a);
					exit(-1);
				} else if (optarg[1] != '\0') {

					fprintf(stderr, "Invalid argument: -t takes exactly one "
									"argument\n");
					argsKill(a);
					exit(1);
				}
				a -> type = optarg[0];
				break;

			case 'p':
				if (optarg[0] == '\0') {

					fprintf(stderr, "Invalid argument: -p takes exactly one "
									 "argument\n");
					argsKill(a);
					exit(1);
				}
				nrthr = strToInt(optarg);
				if (nrthr > 0) {

					a -> nrthr = nrthr - 1;
				} else {

					fprintf(stderr, "Invalid argument: -p must be a positive "
									"integer, which %s is not\n", optarg);
					exit(1);
				}
				break;

			default:
				fprintf(stderr, "Invalid argument: %s\n", optarg);
				exit(1);
				break;
		}
	}

	/* Reading through the rest of argv to get starting directories + target */
	a -> start = smalloc(sizeof(*a -> start) * argc);
	a -> start[0] = NULL;
	for (int i = optind; i < argc; i++) {

		if (i == argc - 1) {

			int strLen = strlen(argv[i]);
			a -> target = smalloc(sizeof(char) * (strLen + 1));
			strcpy(a -> target, argv[i]);
			a -> target[strLen] = '\0';

		} else {

			int strLen = strlen(argv[i]);
			a -> start[a -> nrStart] = malloc(sizeof(char) * (strLen + 1));
			strcpy(a -> start[a -> nrStart], argv[i]);
			a -> start[a -> nrStart][strLen] = '\0';
			a -> nrStart++;
		}
	}

	if (a -> nrStart < 1) {

		fprintf(stderr, "No starting directory, cannot start search\n");
		exit(1);
	}
}

/*
* description: Initiates an args struct with default and NULL values.
* param[in]: a - Pointer to the args struct.
*/
void argsInit (args *a) {

	a -> type = '\0';
	a -> nrthr = 0;
	a -> target = NULL;
	a -> start = NULL;
	a -> nrStart = 0;
}

/*
* description: Free's all memory in an args struct.
* param[in]: a - Pointer to the args struct.
* return:
*/
void argsKill (args *a) {

	if (a != NULL) {

		sfree(a -> target);

		if (a -> start != NULL) {

			for (int i = 0; i < a -> nrStart; i++) {

				sfree(a -> start[i]);
			}
			sfree(a -> start);
		}
	}
}

/*
* description: Converts a string of ascii characthers into an integer.
* param[in]: str - The sring.
* return: The converted integer.
*/
int strToInt (char *str) {

	int sum = 0;
	for (int i = 0; str[i] != '\0'; i++) {

		if (str[i] >= '0' && str[i] <= '9') {

			sum = sum * 10 + str[i] - '0';
		} else {

			return -1;
		}
	}
	return sum;
}
