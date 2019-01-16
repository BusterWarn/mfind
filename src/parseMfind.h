/*
* Parser for mfind.c. Will parse main arguments and read the options, storing
* them in a struct - args.
*
* Author: Buster Hultgren Wärn <dv17bhn@cs.umu.se>
*
* Final build: 2018-10-26
*/

#ifndef __PARSER__
#define __PARSER__

/* Arguments parsed through													*/
typedef struct args {

	char type;
	char *target;
	char **start;
	int nrthr;
	int nrStart;
} args;

/*
* description: Main parser for arguments. Uses getopt to parse flags. Rest of
* the arguments are read as starting positions or the target.
* param[in]: a - Pointer to args struct. Arguments will be stored here.
* param[in]: argc - Number of arguments.
* param[in]: argv - The arguments.
*/
void parseArgs (args *a, int argc, char *argv[]);

/*
* description: Initiates an args struct with default and NULL values.
* param[in]: a - Pointer to the args struct.
*/
void argsInit (args *a);

/*
* description: Free's all memory in an args struct.
* param[in]: a - Pointer to the args struct.
* return:
*/
void argsKill (args *a);

/*
* description: Converts a string of ascii characthers into an integer.
* param[in]: str - The sring.
* return: The converted integer.
*/
int strToInt (char *str);

#endif	//__PARSER__
