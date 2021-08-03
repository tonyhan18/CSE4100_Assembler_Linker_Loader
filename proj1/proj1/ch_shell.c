#include "ch_shell.h"

/* struct */
typedef struct  s_NODE* nptr;
// history linked list node
typedef struct  s_NODE {
    int     node_cnt;
    char    node_ord[ORD_LEN];
    nptr    link;
}               t_NODE;

/* global val */
// number of nodes in the history linked list
int     cnt;
// point head node of the history linked list
nptr    HEAD;

/*------------------------------------------------------------------------------------*/
/*function : help*/
/*purpose : print all the list that can use */
/*return : void
/*------------------------------------------------------------------------------------*/

 // h[elp]
void            help()
{
    printf("h[elp]\n");
    printf("d[ir]\n");
    printf("q[uit]\n");
    printf("hi[story]\n");
    printf("du[mp] [start, end]\n");
    printf("e[dit] address, value\n");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\n");
    printf("opcodelist\n");
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : dir*/
/*purpose : print all the list of files in same directory */
/*return : void
/*------------------------------------------------------------------------------------*/
//d[ir]
void            dir() {
    // open the present working directory
    DIR             *dir_ptr = opendir(".");
    // get a file name
    struct dirent   *file = NULL;
    // get state of the file
    struct stat     file_stat;

    // read the directory
    while ((file = readdir(dir_ptr)))
    { 
        // get state of the file
        lstat(file->d_name, &file_stat);
        // file is directory
        if (S_ISDIR(file_stat.st_mode))
            printf("%s/\n", file->d_name);
        // file is executable file
        else if(file_stat.st_mode & S_IXUSR)
            printf("%s*\n", file->d_name);
        // else
        else printf("%s\n", file->d_name);
    }
    closedir(dir_ptr);
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : push*/
/*purpose : insert the ord */
/*return : void
/*------------------------------------------------------------------------------------*/

// push cmd to the history
void            push(char* ord)
{
    // create the node
    nptr    tmp = (nptr)malloc(sizeof(t_NODE));
    nptr    h;
    // increase the total number of nodes
    tmp->node_cnt = ++cnt;
    // store the cmd
    strcpy(tmp->node_ord, ord);
    tmp->link = NULL;

    // linked list is empty
    if (!HEAD)
        HEAD = tmp;
    else {
        // move while it's not end
        for (h = HEAD; h->link ; h = h->link);
        // push current node
        h->link = tmp;
    }
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : history*/
/*purpose : print all the list that used */
/*return : void
/*------------------------------------------------------------------------------------*/
// hi[story]
void            history()
{
    //if not null it work
    for (nptr h = HEAD; h; h = h->link)
    {
         // print contents of the histiory
        printf("%-5d %s\n", h->node_cnt, h->node_ord);
    }
    return ;
}