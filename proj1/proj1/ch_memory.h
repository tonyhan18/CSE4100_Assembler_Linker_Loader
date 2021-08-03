#ifndef CH_MEMORY_H
# define CH_MEMORY_H

/* include files */
#include <string.h>
#include <stdio.h>

/* constraints */
// define 1MB size of memory
# define MEMORY_SIZE 65536 * 16

/* function */
void dump(int, int);
void edit(int, int);
void fill(int, int, int);
void reset();

# endif