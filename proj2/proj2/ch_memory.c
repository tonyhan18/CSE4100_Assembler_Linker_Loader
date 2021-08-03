#include "ch_memory.h"

/* global variable */
// memory variable
unsigned char   memory[MEMORY_SIZE]; 

/*------------------------------------------------------------------------------------*/
/*function : dump*/
/*purpose : print all the address from start to end */
/*return : void */
/*------------------------------------------------------------------------------------*/

// du[mp], [start], [end]
void            dump(int start, int end)
{
    // left column memory address is Hex
    for (int i = start - (start % 16) ; i <= end ; i += 16)
    {
        // print memory address
        printf("%05X", i);
        // print middle column
        for (int j = 0; j < 16; j++)
        { 
            //others
            printf(" ");
            if (i + j < start || i+j > end)
            {
                printf("  ");
                continue;
            }
            printf("%02X", memory[i + j]);
        }
        // last
        printf(" ; ");

        // print character value of memory addr
        for (int j = 0; j < 16; j++)
        {
            //if not a word that can print
            if (i + j < start || i+j > end
                || memory[i + j] < 0x20 || memory[i + j] > 0x7E)
            {
                printf(".");
                continue;
            }
            //real printable word
            printf("%c", memory[i + j]);
        }
        printf("\n");
    }
    return;
}

/*------------------------------------------------------------------------------------*/
/*function : edit*/
/*purpose : edit val in address */
/*return : void */
/*------------------------------------------------------------------------------------*/

// e[dit]
void            edit(int addr, int val)
{
    // change value
    memory[addr] = val;
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : fill*/
/*purpose : fill the memory from start to end by val */
/*return : void */
/*------------------------------------------------------------------------------------*/

// f[ill]
void            fill(int start, int end, int val)
{
    // change the all values
    for (int i = start; i <= end; i++)
        memory[i] = val;
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : reset*/
/*purpose : reset the memory to 0 */
/*return : void */
/*------------------------------------------------------------------------------------*/

// reset
void reset()
{
    // make the memory to zero
    memset(memory, 0, sizeof(memory));
    return ;
}