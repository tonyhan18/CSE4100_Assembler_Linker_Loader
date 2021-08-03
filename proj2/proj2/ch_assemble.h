#ifndef __CH_ASSEMBLE_H__
# define __CH_ASSEMBLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch_utility.h"
#include "ch_opcode.h"

#define CLOSE       fclose(np);fclose(fp);fclose(tp);
#define CLOSE_ALL   fclose(np);fclose(fp);fclose(tp);fclose(lp);fclose(op);

typedef struct __NODE_SYM* sptr;
// symbol table
typedef struct __NODE_SYM {
    sptr left, right;
	int address;
	char symbol[SYM_LEN];
} NODE_SYM;

// linked list for mrecord 
typedef struct __NODE_MERC* mptr;
typedef struct __NODE_MERC {
    mptr link;
	int mrec;
} NODE_MERC;

/* function */
int     assemble(char*);
void    symbol();
void    initialize();
void    inorder(sptr);
void    insert(sptr*, int, char*);
void    destroy(sptr);
sptr    search_pre(sptr, char*);
sptr     search(sptr, char*);
int     chk_mnemonic(char*);
int     chk_relative(int*, int, int);
int     pass1(char*);
int     pass2(char*);


#endif