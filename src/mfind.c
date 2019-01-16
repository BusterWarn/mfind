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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mfind.h"
#include "queue.h"
#include "parseMfind.h"
#include "saferMemHandler.h"


int main (int argc, char *argv[]) {

	args a;
	argsInit(&a);
	parseArgs(&a, argc, argv);
	if (a.nrStart > 0) {

		runThreads(&a);
	}
	argsKill(&a);
	return 0;
}

/*
* description: Runs all threads (including main) through mfind() and the joins
* them.
* param[in]: a - args struct filled with parsed arguments.
*/
void runThreads (args *a) {

	initMutexAndSem(a -> nrStart);

	pthread_t trd[a -> nrthr];
	trdArgs trdArg;
	trdArg.q = queueEmpty();
	trdArg.target = objectNew(a -> target, a -> type);
	a -> target = NULL;

	printf("\n");
	initQueue(a, trdArg.q, trdArg.target);
	threadsCreate(a -> nrthr, &trdArg, trd);		/* Running threads		*/
	void *reads = mfind(&trdArg);					/* Running main thread 	*/
	printf("\n");
	threadsJoin(a -> nrthr, trd);
	printf("Thread: %ld Reads: %d\n", pthread_self(), *(int *)reads);

	sfree(reads);
	objectKill(trdArg.target);
	queueKill(trdArg.q);
}


/*
* description: Initiates global mutexes mtxQueue, mtxThrCounter and also
* global semaphore semTrdSearch.
* param[in]: semValue - The value to be set on semaphore.
*/
void initMutexAndSem (int semValue) {

	int rc = 0;

	if ((rc = pthread_mutex_init(&mtxQueue, NULL) != 0)) {

		fprintf(stderr, "phtread_mutex_init failed with error code %d\n", rc);
		exit(1);
	}
	if ((rc = pthread_mutex_init(&mtxThrCounter, NULL) != 0)) {

		fprintf(stderr, "phtread_mutex_init failed with error code %d\n", rc);
		exit(1);
	}
	if ((rc = sem_init(&semTrdSearch, 0, semValue)) != 0) {

		fprintf(stderr, "sem_init failed with error code %d\n", rc);
		exit(1);
	}
}

/*
* description: Creates additional (non-main) threads. Threads are created to
* run with mfind.
* param[in]: nrthr - Number of threads to be created.
* param[in]: trdArg - Pointer to thread argument struct (trdArgs).
* param[in]: trd - Array containing uninitiated threads.
*/
void threadsCreate (int nrthr, trdArgs *trdArg, pthread_t trd[]) {

	int rc = 0;
	for (int i = 0; i < nrthr; i++) {

		rc = pthread_create(&trd[i], NULL, mfind, trdArg);
		if (rc != 0) {
			fprintf(stderr, "pthread_create failed with error code %d\n", rc);
			exit(1);
		}
	}
}

/*
* description: Joins additional (non-main) threads. Will print out results from
* mfind (directories searched through) to stdout.
* param[in]: nrthr - Number of threads to be joined.
* param[in]: trd - Array containing initiated threads.
*/
void threadsJoin (int nrthr, pthread_t trd[]) {

	void *reads = NULL;
	for (int i = 0; i < nrthr; i++) {

		pthread_join(trd[i], &reads);
		printf("Thread: %ld Reads: %d\n", trd[i], *(int *)reads);
		sfree(reads);
	}
}

/*
* description: Initiates queue with the starting directories given as argument
* to main. Will also see if starting directories compares equal to the target.
* If so, its complete path will be printed to stdout.
* param[in]: a - Struct containing arguments from parser.
* param[in]: q - The queue.
* param[in]: target - The target.
* return:
*/
void initQueue (args *a, queue *q, object *target) {

	for (int i = 0; i < a -> nrStart; i++) {

		object *o = objectNew(a -> start[i], 'd');
		a -> start[i] = NULL;
		if (objectCmp(target, o)) {

			int nameLen = strlen(o -> name);
			if (o -> name[nameLen - 1] == '/') {

				char buffer[nameLen];
				buffer[nameLen - 1] = '\0';
				strncpy(buffer, o -> name, nameLen - 1);
				printf("%s\n", buffer);
			} else {

				printf("%s\n", o -> name);
			}
		}
		queueEnqueue(q, (void *)o);
	}
}

/*
* description: Runs a thread through trdSearchDir() IF there is an element in
* queue containing directories to look through. IF there is not BUT other
* threads are running trdSearchDir(), thread will wait for signal. Else it will
* quit.
* param[in]: arg - void pointer to Thread argument struct (trdArgs).
* return: Allocated memory storing an integer keeping track of how many
* directories the thread have (succesfully) opened. Should be free'd.
*/
void *mfind (void *arg) {

	int *reads = malloc(sizeof(*reads));
	*reads = 0;
	trdArgs *trdArg = (trdArgs *)arg;
	object *o = NULL;
	int runLoop = 1;

	while (runLoop) {

		sem_wait(&semTrdSearch);

		pthread_mutex_lock(&mtxQueue);
		if (!queueIsEmpty(trdArg -> q)) {

			o = queueFront(trdArg -> q);
			queueDequeue(trdArg -> q);
		}
		pthread_mutex_unlock(&mtxQueue);

		if (o != NULL) {

			*reads += trdSearchDir(trdArg -> q, o, trdArg -> target);
			o = NULL;
		} else {

			pthread_mutex_lock(&mtxThrCounter);
			if (THRSRUNNING == 0) {

				runLoop = 0;
				sem_post(&semTrdSearch);
			}
			pthread_mutex_unlock(&mtxThrCounter);
		}
	}

	return (void *)reads;
}

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
int trdSearchDir (queue *q, object *o, object *target) {

	pthread_mutex_lock(&mtxThrCounter);
	THRSRUNNING++;
	pthread_mutex_unlock(&mtxThrCounter);

	int succesfullRead = 0;
	DIR *dir = opendir(o -> name);
	if (dir == NULL) {

		perror(o -> name);
	} else {

		struct dirent *entry;
		struct stat buf;
		while ((entry = readdir(dir)) != NULL) {

			if (entry -> d_name[0] != '.') {

				char *newPath = objectAddSuffix(o, entry -> d_name);

				if (lstat(newPath, &buf) < 0) {

					perror(newPath);
					sfree(newPath);
				} else {

					if (trdObjectCmp(target, entry -> d_name, &buf)) {

						printf("%s\n", newPath);
					}

					if ((buf.st_mode & S_IFMT) == S_IFDIR) {

						stringAddForwardslashSuffix(newPath);
						if (newPath != NULL) {

							object *newObj = objectNew(newPath, 'd');
							pthread_mutex_lock(&mtxQueue);
							queueEnqueue(q, (void *)newObj);
							pthread_mutex_unlock(&mtxQueue);
							sem_post(&semTrdSearch);
						}
					} else {				/* If entry is not a directory	*/

						sfree(newPath);
					}

				}
			}
		}
		succesfullRead = 1;
		closedir(dir);
	}
	pthread_mutex_lock(&mtxThrCounter);
	THRSRUNNING--;
	pthread_mutex_lock(&mtxQueue);
	if (queueIsEmpty(q) && THRSRUNNING == 0) {

		sem_post(&semTrdSearch);
	}
	pthread_mutex_unlock(&mtxQueue);
	pthread_mutex_unlock(&mtxThrCounter);

	objectKill(o);
	return succesfullRead;
}

/*
* description: From a thread running trdSearchDir(), compares to see if target
* equals one of the entries in directory it's searching.
* param[in]: target - the Target.
* param[in]: entryName - Name of the entry.
* param[in]: buf - Buffer to stat struct containing information about the
* entry. Should be initiated with lstat().
* return: If entry compares equal to target; 1, else 0.
*/
int trdObjectCmp (object *target, char *entryName, struct stat *buf) {

	object entryObj;
	entryObj.name = entryName;
	if ((buf -> st_mode & S_IFMT) == S_IFDIR) {

		entryObj.type = 'd';
	} else if ((buf -> st_mode & S_IFMT) == S_IFREG) {

		entryObj.type = 'f';
	} else if ((buf -> st_mode & S_IFMT) == S_IFLNK) {

		entryObj.type = 'l';
	}
	return objectCmp(target, &entryObj);
}

/*
* description: To a string, adds a forward slash ( '/' ) to it's last position.
* string MUST contain two NULL-bytes ( '\0' ) at it's last two positions.
* param[in]: str - The string.
*/
void stringAddForwardslashSuffix (char *str) {

	int len = strlen(str);
	str[len] = '/';
}

/*
* description: Creates and initiates an object. Allocates memory for it the
* object.
* param[in]: name - Name of the object (no memory will be allocated).
* param[in]: type - The type of object (no memory will be allocated).
* return: Pointer to the object.
*/
object *objectNew (char *name, char type) {

	object *o = smalloc(sizeof(*o));
	o -> name = name;
	o -> type = type;
	return o;
}

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
int objectCmp (object *targetO, object *o) {

	int equals = 0;
	if (targetO -> type == o -> type || targetO -> type == '\0') {

		equals = 1;
		char *name1 = objectGetSuffixIndex(targetO);
		char *name2 = objectGetSuffixIndex(o);

		for (int i = 0; name1[i] != '\0' && name2[i] != '\0' && equals; i++) {

			if (name1[i] != name2[i] || name1[i + 1] != name2[i + 1]) {

				equals = 0;
				if ((name1[i] == '/' && name2[i + 1] == '\0') ||
					(name2[i] == '/' && name1[i + 1] == '\0')) {

						equals = 1;
				}
			}
		}
	}
	return equals;
}

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
char *objectAddSuffix (object *o, char *suffix) {

	int oldLength = strlen(o -> name);
	if (o -> name[oldLength - 1] != '/') {

		oldLength++;
	}
	int newLength =  oldLength + strlen(suffix);


	char *newPath = smalloc(sizeof(char) * (newLength + 2));
	newPath[0] = '\0';
	strcat(newPath, o -> name);
	newPath[oldLength - 1] = '/';
	newPath[oldLength] = '\0';
	strcat(newPath, suffix);
	newPath[newLength] = '\0';
	newPath[newLength + 1] = '\0';

	return newPath;
}

/*
* description: Gets the suffix (part between forward slashes ( / )) of an
* object name and points to, where in the string, the suffix starts. No memory
* will be allocated.
* param[in]: o - The object.
* return: Pointer to where in the objects name the suffix starts.
*/
char *objectGetSuffixIndex (object *o) {

	char *index = &o -> name[0];
	for (int i = 0; o -> name[i] != '\0'; i++) {

		if (o -> name[i] == '/' && o -> name[i + 1] != '\0') {

			index = &o -> name[i + 1];
		}
	}
	return index;
}

/*
* description: Free's an object and the memory allocated for it's name.
* param[in]: o - The object.
*/
void objectKill (object *o) {

	sfree(o -> name);
	sfree(o);
}
