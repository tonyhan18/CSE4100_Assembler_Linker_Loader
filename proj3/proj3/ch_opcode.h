#ifndef CH_OPCODE_H
# define CH_OPCODE_H

/* include files */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ch_utility.h"

/* struct */
typedef struct  s_HASH* hptr;
// linked list of hash node
typedef struct  s_HASH
{
    hptr    link;
    int     fmt;
    char    op[OP_LEN];
    char    mnemonic[MAX_MNEMONIC];
}               t_HASH;

/* function */
int ft_hash(char*);
void table_push(int, char*, char*, int);
int table_make();
int opcode(char*);
int opcodelist();

#endif