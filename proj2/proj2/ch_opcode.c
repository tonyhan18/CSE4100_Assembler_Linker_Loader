#include "ch_opcode.h"

/* global variable */
hptr        hash_table[HASH_LEN];
// if hash table had been constructed it will be 1
int         hash_flag;


/*------------------------------------------------------------------------------------*/
/*function : ft_hash*/
/*purpose : translate key to hash(int) */
/*return : it */
/*------------------------------------------------------------------------------------*/

 // hash function
int         ft_hash(char* key)
{
    int     hash = 0;
    for (int i = 0 ; i < strlen(key) ; i++)
    {
        // error : if not printable
        if (key[i] < 'A' || key[i] > 'Z')
            return -1;
        // printable
        hash += key[i] - 'A';
    }
    return hash % HASH_LEN;
}

/*------------------------------------------------------------------------------------*/
/*function : table_push*/
/*purpose : push op to table */
/*return : void */
/*------------------------------------------------------------------------------------*/

// push a node to the hash table
void        table_push(int hash, char *op, char *mnemonic, int fmt)
{
    // create new node
    hptr    tmp = (hptr)malloc(sizeof(t_HASH));
    hptr    w;

    // insert data
    strcpy(tmp->op, op);
    strcpy(tmp->mnemonic, mnemonic);
    tmp->fmt = fmt;
    tmp->link = NULL;
    
    // number of node is less than 1
    if (!hash_table[hash]) hash_table[hash] = tmp;
    // number of node is more than 1
    else
    {
        // go to last node
        for (w = hash_table[hash] ; w->link ; w = w->link);
        // push current node to tha last
        w->link = tmp;
    }
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : table_make */
/*purpose : if table is not exist it makes table */
/*return : int */
/*------------------------------------------------------------------------------------*/

// make and initialize the hash table
int         table_make()
{
    // lines in the file
    char    line[MAX_LINE];
    char*   op;
    char*   mnemonic;
    char*   format;
    // end of line
    // it will check whether the line has appropriate number of columns
    char*   EOL; 
    int     hash;

    // read file
    FILE *fp = fopen("opcode.txt", "r");
    // error : not exist opcode file or empty
    if (!fp) {
        printf("Opcode File Doesn't Exist!\n");
        return 0;
    }

     // get a line until EOF
    while (fgets(line, MAX_LINE, fp))
    {
        // change newline to null in last
        line[strlen(line) - 1] = '\0';
        // get opcode from the line
        op = strtok(line, LIMITER);
        if (!op) break;
        // get mnemonic from the line
        mnemonic = strtok(NULL, LIMITER);
        format = strtok(NULL, LIMITER);
        EOL = strtok(NULL, LIMITER);
        // error : invalid number of columns
        if (!format || !mnemonic || EOL)
        {
            printf("Invalid Opcode File!\n");
            fclose(fp);
            return 0;
        }
        // error : hash is error
        if ((hash = ft_hash(mnemonic))==-1)
        {
            printf("Invalid Opcode File!\n");
            return 0;
        }
        table_push(hash, op, mnemonic, format[0] - '0');
    }
    fclose(fp);
    // hash_table has been constructed successfully 
    return (1);
}

/*------------------------------------------------------------------------------------*/
/*function : opcode*/
/*purpose : print mnemonic's opcode */
/*return : int */
/*------------------------------------------------------------------------------------*/

// when cmd is opcode mnemonic
int         opcode(char *mnemonic)
{
    // hash table doesn't exist
    if (!hash_flag)
    {
        if (table_make())
            hash_flag = 1;
        else return (0);
    }
    int     hash;
    hptr    h;
    hash = ft_hash(mnemonic);

    // search function
    for (h = hash == -1 ? NULL : hash_table[hash]; h; h = h->link)
    {
        // found
        if (!strcmp(h->mnemonic, mnemonic))
        {
            printf("opcode is %s\n", h->op);
            return (1);
        }
    }
    //not founded
    printf("Invalid Mnemonic!\n");
    return (0);
}

/*------------------------------------------------------------------------------------*/
/*function : opcodelist*/
/*purpose : print all the list that can use */
/*return : int */
/*------------------------------------------------------------------------------------*/

// opcodelist
int         opcodelist()
{
    hptr    h;
    // hash table doesn't exist it works
    if (!hash_flag)
    {
        if (table_make())
            hash_flag = 1;
        else return (0);
    }
    // print all
    for (int i = 0; i < HASH_LEN; i++)
    {
        printf("%d : ", i);

        h = hash_table[i];
        if (!h)
        {
            printf("\n");
            continue;
        }
        printf("[%s,%s]", h->mnemonic, h->op);

        //move with linked list
        for (h = h->link; h; h = h->link)
            printf(" -> [%s,%s]", h->mnemonic, h->op);
        printf("\n");
    }
    return (1);
}