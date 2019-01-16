/*
* Memery allocation and free function helper. After a memory allocation, will
* check that a memory is not NULL. If it is - message will be printed to stderr
* and program will exit with errno code.
*
* On a free, will check that memory is not pointing to NULL, so that a double
* free can occur.
*
* Author: Buster Hultgren Wärn <dv17bhn@cs.umu.se>
*
* Final build: 2018-10-26
*/

#ifndef __SAFERMEMHANDLER__
#define __SAFERMEMHANDLER__

/*
* description: Allocates memory via malloc.
* param[in]: size - The size of the memory to be allocated in bytes.
* return: Void pointer to the allocated memory.
*/
void *smalloc (size_t size);

/*
* description: Allocates memory via calloc.
* param[in]: nmeb - Number of elements to be allocated.
* param[in]: size - The size of each element.
* return: Void pointer to the allocated memory.
*/
void *scalloc (size_t nmeb, size_t size);

/*
* description: Allocates memory via realloc.
* param[in]: ptr - Pointer to original memory that is to be reallocated.
* param[in]: size - The size of the new allocated memory.
* return: Void pointer to the allocated memory.
*/
void *srealloc (void *ptr, size_t size);

/*
* description: Free's a memory block after checking if it leads to NULL.
* param[in]: meme - The block of memory.
*/
void sfree (void *mem);

#endif //__SAFERMEMHANDLER__
