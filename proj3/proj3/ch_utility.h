#ifndef __CH_UTILITY_H__
# define __CH_UTILITY_H__

/* constraints */
//max ord size
# define LIMITER " \t\r\n\f\v"
//# define ORD_LEN 81
# define CMD_LEN 81
# define MAX_LINE 81
// define 1MB size of memory
# define MEMORY_SIZE 65536 * 16
# define MAX_MEMORY 0xFFFFF
# define MAX_VALUE 0xFF
/* constraints */
# define SYM_LEN 6
# define HASH_LEN 20
# define OP_LEN 3
# define MAX_MNEMONIC 11
# define LOC_LEN 7
# define TREC_LEN 20
//loader
# define MAX_REF 50


/* function */
int     hex_to_int(char *, int);
int     dec_to_int(char *, int);

#endif