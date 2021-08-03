#ifndef __CH_LOADER_H__
#define __CH_LOADER_H__

#include <stdio.h>
#include <string.h>
#include "ch_utility.h"
#include "ch_opcode.h"

typedef struct _ES_NODE_* eptr;
typedef struct _ES_NODE_ //external symbol table
{
    eptr    link;
    int     address;
    int     len;
    char    sym[SYM_LEN+1];
} ES_NODE;

void    progaddr(char*);
int     loader(char*, char*, char*);
void    es_table_push(int, char*,int,int);
void    es_table_del(void);
int     load_pass1(char*[3]);
int     load_pass2(char*[3]);
void    bp(char*);
void    run(void);

#endif