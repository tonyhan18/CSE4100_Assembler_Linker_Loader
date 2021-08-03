#ifndef MAIN_H
# define MAIN_H

/* include files */
#include <string.h>
#include <stdio.h>
#include "ch_opcode.h"
#include "ch_memory.h"
#include "ch_shell.h"

/* constraints */
# define CMD_LEN 81
# define MAX_MEMORY 0xFFFFF
# define MAX_VALUE 0xFF
# define LIMITER " \t\r\n\f\v"

/* function */
int     hex_to_int(char *str, int idx);

#endif