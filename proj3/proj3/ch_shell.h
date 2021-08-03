#ifndef CH_SHELL_H
# define CH_SHELL_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "ch_utility.h"

/* struct */
typedef struct  s_NODE* nptr;
// history linked list node
typedef struct  s_NODE {
    int     node_cnt;
    char    node_ord[CMD_LEN];
    nptr    link;
}               t_NODE;

void    help();
void    dir();
void    push(char*);
void    history();
int     type(char*);

# endif