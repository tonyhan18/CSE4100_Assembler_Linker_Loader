#ifndef CH_OPCODE_H
# define CH_OPCODE_H

/* include files */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* constraints */
# define MAX_LINE 81
# define HASH_LEN 20
# define OP_LEN 3
# define MAX_MNEMONIC 11
# define LIMITER " \t\r\n\f\v"

/* function */
int ft_hash(char*);
void table_push(int, char*, char*);
int table_make();
int opcode(char*);
int opcodelist();

#endif