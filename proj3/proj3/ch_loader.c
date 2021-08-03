#include "ch_loader.h"

// SIC/XE registers number 
typedef enum {A,X,L,B,S,T,F,NAN,PC,SW} reg;
// progaddr's global variable that stored starting address
int     progaddress;
// loader's global variable that store the value of registers
int     regVal[10];
eptr    es_table[HASH_LEN];
// pass1's global variable
int     csaddress,cslth;
// pass2's global variable
int     execaddress;
extern unsigned char   memory[MEMORY_SIZE]; 
// bp's global variable
int     brkpnt[MEMORY_SIZE];
// run's global variable
int     pre_bp; // restart the program from previous breakpoint


/*------------------------------------------------------------------------------------*/
/*function : progaddr */
/*purpose : set start address when operating loader or run func */
/*return : void */
/*------------------------------------------------------------------------------------*/
void    progaddr(char * addr)
{
    //change addr to hexa
    progaddress = hex_to_int(addr, strlen(addr) -1);
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : loader */
/*purpose : doing pass1, pass2 and make external symbol's hash table */
/*return : int */
/*------------------------------------------------------------------------------------*/
int     loader(char* file1, char* file2, char* file3)
{
    //make files in one array
    char *filenames[3] = {file1, file2, file3};
    //initialize registers
    memset(regVal, 0, sizeof(regVal));
    //initialize external symbol table
    es_table_del();
    //do pass1 if not happened errer
    if(!load_pass1(filenames)) return (0);
    //do pass2 if not happened errer
    if(!load_pass2(filenames)) return (0);
    //program successs
    return (1);
}

/*------------------------------------------------------------------------------------*/
/*function : es_table_push */
/*purpose : push a node to the hash table */
/*return : void */
/*------------------------------------------------------------------------------------*/
void    es_table_push(int hash, char * sym, int address, int len)
{
    eptr temp = (eptr)malloc(sizeof(ES_NODE)); // new node
    strcpy(temp->sym, sym);
    temp->address = address;
    temp->len = len;
    temp->link = NULL;
    //if there is no node
    if (!es_table[hash])
    {
        es_table[hash] = temp;
    }
    else
    {
        eptr cur;
        for (cur = es_table[hash]; cur->link; cur = cur->link); // adding a new node to the last
        cur->link = temp;
    }
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : es_table_del */
/*purpose : delete external symbol hash table */
/*return : void */
/*------------------------------------------------------------------------------------*/
void    es_table_del(void)
{
    //move all es symbol hash table and delete
    for(int i = 0 ; i < HASH_LEN ; ++i)
    {
        eptr nptr;
        for(eptr cptr = es_table[i] ; cptr ; cptr = nptr)
        {
            nptr = cptr->link;
            free(cptr);
        }
        //make NULL
        es_table[i] = NULL;
    }
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : load_pass1 */
/*purpose :  read E, H, D record make external symbol table*/
/*return : int */
/*------------------------------------------------------------------------------------*/
int     load_pass1(char * file[3])
{
    char    *token;
    int     hash;
    char    len[SYM_LEN + 1];
    char    line[MAX_LINE];
    char    sym[SYM_LEN + 1];
    FILE    *fp;

    //set Control Section Address
    csaddress = progaddress;
    //set PC register
    regVal[PC] = progaddress;

    printf("control symbol address length\n");
    printf("section name\n");
    printf("-----------------------------\n");

    //iterate files
    for(int file_idx = 0; file_idx < 3 && file[file_idx]; file_idx++)
    {
        if(!(fp = fopen(file[file_idx], "r")))
        {
            //error check
            printf("File Doesn't Exist!\n");
            return (0);
        }
        // read a file line by line
        while(fgets(line, MAX_LINE, fp))
        {
            line[strlen(line) - 1] = 0;
            //if line is End than stop
            if (line[0] == 'E') break;
            // if line is Head
            else if(line[0] == 'H')
            {
                strncpy(sym, &line[1], SYM_LEN); // get program name
                token = strtok(sym, LIMITER); // remove spaces
                cslth = hex_to_int(&line[SYM_LEN*2 + 1], SYM_LEN - 1); // get progame length
                if ((hash = ft_hash(token)) == -1)//get hash value
                {
                    printf("Invalid Control Section!\n");
                    return (0);
                }
                //search for tokken if there is a dup
                for(eptr cur = es_table[hash]; cur; cur = cur->link)
                {
                    if(!strcmp(cur->sym, token))
                    {
                        //error
                        printf("Duplicated External Symbol!\n");
                        return (0);
                    }
                }
                printf("%-6s           %04X   %04X\n", token, csaddress, cslth);
                es_table_push(hash, token, csaddress, cslth);
            }
            // if line is D
            else if(line[0] == 'D')
            {
                // (symbol name, address) tuple reading
                for(int i = 1 ; i<strlen(line) ; i += SYM_LEN * 2)
                {
                    strncpy(sym, &line[i], SYM_LEN); // get name
                    token = strtok(sym, LIMITER); // remove whilespace
                    strncpy(len, &line[i + SYM_LEN], SYM_LEN); // get address
                    //get hash val and chk
                    if((hash=ft_hash(token)) == -1)
                    {
                        printf("Invalid Control Section!\n");
                        return (0);
                    }
                    for(eptr cur = es_table[hash]; cur; cur = cur->link)
                    {
                        //found symbol
                        if(!strcmp(cur->sym, token))
                        {
                            printf("Duplicated External Symbol!\n");
                            return (0);
                        }
                    }
                    printf("         %-6s  %04X\n", token, csaddress + hex_to_int(len, SYM_LEN - 1));
                    es_table_push(hash, token, csaddress + hex_to_int(len, SYM_LEN - 1), -1); // push tokken to the ES table
                }
            }
        }
        //move to the next
        csaddress += cslth;
    }
    regVal[L] = csaddress - progaddress;
    printf("-----------------------------\n");
    printf("           total length %04X\n", regVal[L]);
    return (1);
}

/*------------------------------------------------------------------------------------*/
/*function : load_pass2 */
/*purpose :  read E, H, R, T, M record and save control section's address*/
/*return : int */
/*------------------------------------------------------------------------------------*/
int     load_pass2(char * file[3])
{
    char    *token;
    int     idx, len, hash, n, x, y;
    char    sym[SYM_LEN + 1];
    char    addr[SYM_LEN + 1];
    char    line[MAX_LINE + 1];
    FILE    *fp;

    csaddress = progaddress;
    execaddress = progaddress;
    for(int file_idx = 0 ; file_idx < 3 && file[file_idx]; file_idx++)
    {
        // save the reference number of R and M record
        char    ref_idx[MAX_REF][SYM_LEN + 1];
        if(!(fp = fopen(file[file_idx], "r")))
        {
            printf("File Doesn't Exits\n");
            return (0);
        }
        while(fgets(line, MAX_LINE, fp))
        {
            line[strlen(line) - 1] = 0;
            // E
            if(line[0] == 'E')
            {
                execaddress = hex_to_int(&line[1], SYM_LEN - 1);
                break ;
            }
            // H
            else if (line[0] == 'H')
            {
                strncpy(sym, &line[1], SYM_LEN); //get name
                token = strtok(sym, LIMITER); //remove whitespace
                strcpy(ref_idx[1], token); //current program's ref num 1
                cslth = hex_to_int(&line[SYM_LEN * 2 + 1], SYM_LEN - 1);
            }
            // R
            else if(line[0] == 'R')
            {
                for(int i = 1 ; i < strlen(line); i+=8) // read one tuple : (reference number, symbol name)
                {
                    idx = dec_to_int(&line[i], 1); //get ref num
                    strncpy(sym, &line[i + 2], SYM_LEN); //get sym name
                    token = strtok(sym, LIMITER); //remove whitespace
                    strcpy(ref_idx[idx], token); //save information
                }
            }
            // T
            else if (line[0] == 'T')
            {
                strncpy(addr, &line[1], SYM_LEN); // current record's start address 
                idx = csaddress + hex_to_int(addr, SYM_LEN-1); // add control section address to the address
                for (int i = SYM_LEN + 3; i < strlen(line) ; i += 2)
                {
                    memory[idx++] = hex_to_int(&line[i], 1); // Save the information to the memory
                }
            }
            // M
            else if (line[0] == 'M')
            {
                strncpy(addr, &line[1], SYM_LEN); // current record's start address 
                idx = csaddress + hex_to_int(addr, SYM_LEN - 1); // add control section address to the address
                len = hex_to_int(&line[SYM_LEN + 1], 1); // check the how many half byte shold be changed
                n = dec_to_int(&line[SYM_LEN + 4], 1); // get ref number
                
                // chk error
                // calculate hash value failed
                if ((hash = ft_hash(ref_idx[n])) == -1)
                {
                    printf("Invalid Control Section!\n"); 
                    return (0);
                }
                // store the address of external symbol
                y = -1;
                for (eptr cur = es_table[hash]; cur; cur = cur->link)
                {
                    // if found
                    if (!strcmp(cur->sym, ref_idx[n]))
                    {
                        //save address
                        y = cur->address;
                        break;
                    }
                }
                // not found
                if (y == -1)
                {
                    printf("Undefined External Symbol!\n");
                    return (0);
                }
                // store the value that will be changed
                x = 0;
                // get the value
                for (int i = 0; i < (len % 2) + (len / 2); i++)
                {
                    x *= 0x100;
                    if (!i && (len % 2))
                    {
                        x += memory[idx+i] % 0x10;
                    }
                    else
                    {
                        x += memory[idx + i];
                    }
                }
                // calculate changed value
                // add
                if (line[9] == '+')
                {
                    x += y;
                }
                // minus                
                else
                {
                    x -= y;
                }
                // as 2'complement form to represent negative numbers 
                unsigned int ux = x;
                // load the changed value
                for (int i = len/2 - 1 + (len % 2); i >= 0; i--)
                {
                    if (!i && (len%2))
                    {
                        memory[idx + i] = (memory[idx+i] / 0x10) * 0x10 + ux % 0x10;
                    }
                    else
                    {
                        memory[idx+i] = ux % 0x100;
                    }
                    ux /= 0x100;
                }
            }
        }
        // move next address
        csaddress += cslth;
    }
    return (1);
}

/*------------------------------------------------------------------------------------*/
/*function : bp */
/*purpose : set breakpoint when it comes to run function it will be works*/
/*return : void */
/*------------------------------------------------------------------------------------*/
void    bp(char * opt)
{
    // if there is no operand
    // print existing break points
    if (!opt) { 
        printf("\tbreakpoint\n");
        printf("\t----------\n");
        for (int i = 0; i < MEMORY_SIZE; i++)
        {
            if (brkpnt[i] > 0)
            {
                printf("\t%X\n", i);
            }
        }
    }
    // clear
    else if (!strcmp(opt, "clear"))
    { 
        memset(brkpnt, 0, sizeof(brkpnt));
        printf("\t[\x1b[32mok\x1b[0m] clear all breakpoints\n");
    }
    // set
    else
    {
        int idx = hex_to_int(opt, strlen(opt) - 1);
        brkpnt[idx] = 1;
        printf("\t[\x1b[32mok\x1b[0m] create breakpoint %x\n", idx);
    }
}

/*------------------------------------------------------------------------------------*/
/*function : run */
/*purpose : run the program using instructions and registers*/
/*return : void */
/*------------------------------------------------------------------------------------*/
void    run(void)
{
    regVal[PC] = pre_bp;
    for (int i = pre_bp; (i < csaddress) && (i == pre_bp || brkpnt[i] != 1); i = regVal[PC])
    {
        
        // format2
        regVal[PC] += 2; // increase program counter
        // CLEAR
        if (memory[i] == 0xB4)
        {
            regVal[memory[i + 1] / 0x10] = 0;
            continue ;
        }
        // COMPR
        if (memory[i] == 0xA0)
        {
            regVal[SW] = regVal[memory[i + 1] / 0x10]
                        - regVal[memory[i + 1] % 0x10];
            continue ;
        }
        // TIXR
        if (memory[i] == 0xB8)
        {
            regVal[X]++;
            regVal[SW] = regVal[X] - regVal[memory[i + 1] / 0x10];
            continue ;
        }
        // format3,4
        int  value = 0, e = (memory[i + 1] & 16) / 16, nxt_inst = memory[i] % 4, target;
        // increase 3 if format3, increase 4 if format4
        regVal[PC] += 1 + e;
        // format 4 : e register set
        if (e)
        {
            target = (memory[i + 1] % 0x10) * 0x10000 + memory[i + 2] * 0x100 + memory[i + 3];
        }
        // format3 : e register set
        else
        {
            target = (memory[i + 1] % 0x10) * 0x100 + memory[i+2];
        }

        // p register set : use 2's complement
        if (memory[i+1] & 32)
        {
            target = regVal[PC] 
                    + (target & 0x800 ? ((target - 1)^0xFFF) * (-1) : target);
        }
        // b register set
        if (memory[i + 1] & 64)
        {
            target += regVal[B];
        }
        // x register set
        if (memory[i + 1] & 128)
        {
            target += regVal[X];
        }
        
        ////// J instructions ///////
        // simple addressing
        if (nxt_inst == 2)
        {
            value = memory[target] * 0x10000
                    + memory[target + 1]*0x100 + memory[target + 2];
        }
        // immediate addressing
        else if (nxt_inst == 3)
        {
            value = target;
        }
        // J
        if ((memory[i] - nxt_inst) == 0x3C)
        {
            regVal[PC] = value;
            continue ;
        }
        // JEQ
        else if ((memory[i] - nxt_inst) == 0x30)
        {
            if (!regVal[SW])
            {
                regVal[PC] = value;
                continue;
            }
        }
        // JLT
        else if ((memory[i] - nxt_inst) == 0x38)
        {
            if (regVal[SW] < 0)
            {
                regVal[PC] = value;
                continue ;
            }
        }
        // JSUB
        else if ((memory[i] - nxt_inst) == 0x48)
        {
            regVal[L] = regVal[PC];
            regVal[PC] = value;
            continue ;
        }

        // else instructions : normal instructions
        // simple addressing
        if (nxt_inst == 1)
        {
            value = target;
        }
        // indirect addressing
        else if (nxt_inst == 2)
        {
            value = memory[memory[target]];
        }
        // simple addressing
        else if (nxt_inst == 3)
        {
            value = memory[target];
        }

        // LDCH
        if ((memory[i] - nxt_inst) == 0x50)
        {
            regVal[A] = (regVal[A] / 0x100) * 0x100;
            regVal[A] += value;
        }
        // RD
        else if ((memory[i] - nxt_inst) == 0xD8)
        {
            regVal[A] = (regVal[A] / 0x100) * 0x100;
        }
        // RSUB
        else if ((memory[i] - nxt_inst) == 0x4C)
        {
            regVal[PC] = regVal[L];
            continue ;
        }
        // STA
        else if ((memory[i] - nxt_inst) == 0x0C)
        {
            memory[target] = regVal[A] / 0x10000;
            memory[target + 1] = (regVal[A] % 0x10000) / 0x100;
            memory[target + 2] = regVal[A] % 0x100;
        }
        // STCH
        else if ((memory[i] - nxt_inst) == 0x54)
        {
            memory[target] = regVal[A] % 0x100;
        }
        // STL
        else if ((memory[i] - nxt_inst) == 0x14)
        {
            memory[target] = regVal[L] / 0x10000;
            memory[target + 1] = (regVal[L] % 0x10000) / 0x100;
            memory[target + 2] = regVal[L] % 0x100;
        }
        // STX
        else if ((memory[i] - nxt_inst) == 0x10)
        {
            memory[target] = regVal[X] / 0x10000;
            memory[target + 1] = (regVal[X] % 0x10000) / 0x100;
            memory[target + 2] = regVal[X] % 0x100;
        }
        // TD
        else if ((memory[i] - nxt_inst) == 0xE0)
        {
            regVal[SW] = -1;
        }
        
        // 3 bytes value used instructions
        // indirect addressing
        if (nxt_inst == 2)
        {
            value = memory[memory[target]] * 0x10000
                    + memory[memory[target] + 1] * 0x100
                    + memory[memory[target] + 2];
        }
        // simple addressing
        else if (nxt_inst == 3)
        {
            value = memory[target] * 0x10000
                    + memory[target + 1] * 0x100
                    + memory[target + 2];
        }

        // COMP
        if ((memory[i] - nxt_inst) == 0x28)
        { 
            regVal[SW] = regVal[A] - value;
        }
        // LDA
        else if ((memory[i] - nxt_inst) == 0x00)
        {
            regVal[A] = value;
        }
        // LDB
        else if ((memory[i] - nxt_inst) == 0x68)
        {
            regVal[B] = value;
        }
        //LDT
        else if ((memory[i] - nxt_inst) == 0x74)
        {
            regVal[T] = value;
        }
    }
    printf("A : %06X  X : %06X\n", regVal[A], regVal[X]);
    printf("L : %06X PC : %06X\n", regVal[L], regVal[PC]);
    printf("B : %06X  S : %06X\n", regVal[B], regVal[S]);
    printf("T : %06X\n", regVal[T]);
    if (brkpnt[regVal[PC]])
    {
        pre_bp = regVal[PC]; // save the pre_bp
        brkpnt[regVal[PC]] = -1; // make void eternal break at loop
        printf("\t\tStop at checkpoint[%X]\n", regVal[PC]);
    }
    else
    {
        pre_bp = progaddress; // when progam has done reset pre_bp
        printf("\t\tEnd Program\n");
    }
}