/*
* mfind - Find a specific file, link or directory from a starting directory
* tree.
*
* Synopsis: mfind [-t type] [-p nrthr] start1 [start2 ...] target
*
* -t		Type of target to find. f=file, d=directory, l=link. If empty,
* mfind will search for any of these.
*
* -p		Number of threads that mfind will use to search for the target.
* Default value is 1. Must be a positive integer.
*
* start		Starting directory to begin search from. Must be one or more
* starting directories.
*
* target	The name of the target file/directory/link that mfind will search
* for. There must be exactly one target.
*
* Author: Buster Hultgren Wärn <dv17bhn@cs.umu.se>
*
* Date: 2018-10-26
*
* Modified by: Buster Hultgren Wärn
* Date: 2018-11-13
* What? Changed condition lock condQueue to a semaphore mtxThrCounter to fix
* some errors. Also changed function initMutexAndCond to initMutexAndSem - the
* function now initiates a semaphore instead of a condition lock, and it takes
* one argument - semValue.
*/

#ifndef __MFIND__
#define __MFIND__

// #include "queue.h"

/* Number of threads currently looking through a directory 					*/
static int THRSRUNNING;

/* Mutex variables for locking shared queue or global THRSRUNNING variable 	*/
pthread_mutex_t mtxQueue;
pthread_mutex_t mtxThrCounter;

/* Semanphore for searching threads. Should be same as objects in queue		*/
sem_t semTrdSearch;

/* Typedefs for structs declared other files								*/
typedef struct args args;
typedef struct queue queue;

/* Object file/directory/link - contains name and type 						*/
typedef struct object {

	char *name;
	char type;
} object;

/* Arguments for each thread - contains a queue and the target 				*/
typedef struct trdArgs {

	queue *q;
	object *target;
} trdArgs;

/*
* description: Runs all threads (including main) through mfind() and the joins
* them.
* param[in]: a - args struct filled with parsed arguments.
*/
void runThreads (args *a);

/*
* description: Initiates global mutexes mtxQueue, mtxThrCounter and also
* global semaphore semTrdSearch.
* param[in]: semValue - The value to be set on semaphore.
*/
void initMutexAndSem (int semValue);

/*
* description: Creates additional (non-main) threads. Threads are created to
* run with mfind.
* param[in]: nrthr - Number of threads to be created.
* param[in]: trdArg - Pointer to thread argument struct (trdArgs).
* param[in]: trd - Array containing uninitiated threads.
*/
void threadsCreate (int nrthr, trdArgs *trdArg, pthread_t trd[]);

/*
* description: Joins additional (non-main) threads. Will print out results from
* mfind (directories searched through) to stdout.
* param[in]: nrthr - Number of threads to be joined.
* param[in]: trd - Array containing initiated threads.
*/
void threadsJoin (int nrthr, pthread_t trd[]);

/*
* description: Initiates queue with the starting directories given as argument
* to main. Will also see if starting directories compares equal to the target.
* If so, its complete path will be printed to stdout.
* param[in]: a - Struct containing arguments from parser.
* param[in]: q - The queue.
* param[in]: target - The target.
* return:
*/
void initQueue (args *a, queue *q, object *target);

/*
* description: Runs a thread through trdSearchDir() IF there is an element in
* queue containing directories to look through. IF there is not BUT other
* threads are running trdSearchDir(), thread will wait for signal. Else it will
* quit.
* param[in]: arg - void pointer to Thread argument struct (trdArgs).
* return: Allocated memory storing an integer keeping track of how many
* directories the thread have (succesfully) opened. Should be free'd.
*/
void *mfind (void *arg);

/*
* description: With one thread, searches through an entire directory. Each
* entry in the directory will be compared to the target - if they equal, the
* etnry's path will be printed. If the entry is a directory, it will be added
* as an element to the queue.
* param[in]: q - The queue.
* param[in]: o - The directory to be searched.
* param[in]: target - The target.
* return: If directory is succesfully opened; 1, else 0.
*/
int trdSearchDir (queue *q, object *o, object *target);

/*
* description: From a thread running trdSearchDir(), compares to see if target
* equals one of the entries in directory it's searching.
* param[in]: target - the Target.
* param[in]: entryName - Name of the entry.
* param[in]: buf - Buffer to stat struct containing information about the
* entry. Should be initiated with lstat().
* return: If entry compares equal to target; 1, else 0.
*/
int trdObjectCmp (object *target, char *entryName, struct stat *buf);

/*
* description: To a string, adds a forward slash ( '/' ) to it's last position.
* string MUST contain two NULL-bytes ( '\0' ) at it's last two positions.
* param[in]: str - The string.
*/
void stringAddForwardslashSuffix (char *str);

/*
* description: Creates and initiates an object. Allocates memory for it the
* object.
* param[in]: name - Name of the object (no memory will be allocated).
* param[in]: type - The type of object (no memory will be allocated).
* return: Pointer to the object.
*/
object *objectNew (char *name, char type);

/*
* description: Compares two objects to see if they equal. If their suffixes
* match (part between forward slashes ( / )), they equal. If target object's
* type is '\0', second object's type will be ignored. If an object name
* contains an additional forward slash, this will be ignored.
* param[in]: targetO - The first object to be compared - also the target
* object.
* param[in]: cmpObj - The second object to be compared.
* return: If objects equal; 1, else 0.
*/
int objectCmp (object *targetObj, object *cmpObj);

/*
* description: Adds a suffix (part between forward slashes ( / )) to an object.
* New memory will be allocated for the new and longer name, but the objects
* name will not be altered. At least one forward slash will separete the object
* name from the suffix. The new name will contain two NULL-bytes ( '\0' ) at
* its end.
* param[in]: o - The object.
* param[in]: suffix - The suffix.
* return: Pointer to the new object name.
*/
char *objectAddSuffix (object *o, char *suffix);

/*
* description: Gets the suffix (part between forward slashes ( / )) of an
* object name and points to, where in the string, the suffix starts. No memory
* will be allocated.
* param[in]: o - The object.
* return: Pointer to where in the objects name the suffix starts.
*/
char *objectGetSuffixIndex (object *o);

/*
* description: Free's an object and the memory allocated for it's name.
* param[in]: o - The object.
*/
void objectKill (object *o);

#endif	//__MFIND__
