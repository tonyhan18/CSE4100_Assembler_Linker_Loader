#ifndef CH_SHELL_H
# define CH_SHELL_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

//max ord size
#define ORD_LEN 81

void help();
void dir();
void push(char*);
void history();

# endif