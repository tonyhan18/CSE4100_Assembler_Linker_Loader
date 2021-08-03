#include "ch_assemble.h"

//extern hash_flag which is declared in ch_opcode.c
extern int  hash_flag;
extern hptr hash_table[HASH_LEN];
//complited symbol table
sptr sym_table;
//current symbol table
sptr cur;
// register symbol table
sptr reg;


/*------------------------------------------------------------------------------------*/
/*function : edit*/
/*purpose : edit val in address */
/*return : void */
/*------------------------------------------------------------------------------------*/
int     assemble(char* file)
{
    // if hash table is not exist it will make new
    if(!hash_flag)
    {
        // if it was made well...
        if(table_make()) hash_flag = 1;
        //error occured
        else return (0);
    }
    initialize();
    
    // if assemble file has error occurred
    if (!pass1(file)) {
        //remove all the immediate files
        remove("TEMP.txt");
        remove("newLine.txt");
        return (0);
    }

    // assemble success
    int ans;
    if ((ans = pass2(file)))
    {
        // destroy previous symbol table
        if (sym_table) 
            destroy(sym_table);
        // store new symbol table
        sym_table = cur;

        // reset current symbol table
        cur = NULL;
    }
    //program end
    // remove immediate files
    remove("TEMP.txt");
    remove("newLine.txt");
    return (ans);
}

/*------------------------------------------------------------------------------------*/
/*function : symbol */
/*purpose : display symbols */
/*return : void */
/*------------------------------------------------------------------------------------*/
void    symbol(void)
{
    inorder(sym_table);
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : initialize*/
/*purpose : init before assembling */
/*return : void */
/*------------------------------------------------------------------------------------*/
void    initialize(void)
{
    //if previous assembling has failed it is going to remove
    destroy(cur);
    cur = NULL;

    // if register
    if(reg) return;
    //make register
    else
    {
        insert(&reg, 0, "A");
        insert(&reg, 1, "X");
        insert(&reg, 2, "L");
        insert(&reg, 3, "B");
        insert(&reg, 4, "S");
        insert(&reg, 5, "T");
        insert(&reg, 6, "F");
        insert(&reg, 8, "PC");
        insert(&reg, 9, "SW");
        return ;
    }
}

/*------------------------------------------------------------------------------------*/
/*function : inorder */
/*purpose : search all the BST by inorder */
/*return : void */
/*------------------------------------------------------------------------------------*/
void    inorder(sptr head)
{
    if(!head)
        return ;
    inorder(head -> left);
    printf("        %-8s %04X\n", head -> symbol, head -> address);
    inorder(head -> right);
}

/*------------------------------------------------------------------------------------*/
/*function : insert */
/*purpose : make and insert new node to BST */
/*return : void */
/*------------------------------------------------------------------------------------*/
void    insert(sptr* root, int address, char* symbol)
{
    sptr    successor;
    sptr    predecessor = search_pre(*root, symbol);

    // predecessor is exist or BST is empty it works
    if (!*root || predecessor)
    {
        // make node and initialize
		successor = (sptr)malloc(sizeof(NODE_SYM));
		successor->address = address;
		strcpy(successor->symbol, symbol);
		successor->left = successor->right = NULL;

        // if predecessor found it will put node
		if (*root) {
            //successor is right child of predecessor
            if (strcmp(symbol, predecessor->symbol) >= 0)
				predecessor->right = successor; 
            // successor is left child of predecessor
			else predecessor->left = successor;
		}
		else *root = successor; // empty BST
	}
    return ;
}

/*------------------------------------------------------------------------------------*/
/*function : destroy*/
/*purpose : destroy BST by postorder */
/*return : void */
/*------------------------------------------------------------------------------------*/
void    destroy(sptr root)
{
    if(!root) return ;
    destroy(root->left);
    destroy(root->right);
    free(root);
}
/*------------------------------------------------------------------------------------*/
/*function : search_pre */
/*purpose : search for predecessor and return address*/
/*return : sptr */
/*------------------------------------------------------------------------------------*/
sptr   search_pre(sptr root, char* key)
{
    // save predecessor address
    sptr predecessor = NULL; 

    //using while loop to find predecessor
	while (root) {
        // check if it is duplicated
		if (!strcmp(key, root->symbol)) 
            return NULL; 

        //left
		if (strcmp(key, root->symbol) < 0) {
			predecessor = root;
			root = root->left;
		}
        //right
		else { 
			predecessor = root;
			root = root->right;
		}
	}
	return (predecessor);
}

/*------------------------------------------------------------------------------------*/
/*function : search */
/*purpose : search for the node */
/*return : sptr */
/*------------------------------------------------------------------------------------*/
sptr   search(sptr root, char* key)
{
    // not found
    if (!root)
        return (NULL);
    // found
    if (!strcmp(key, root->symbol))
        return (root);

    // go right
	if (strcmp(key, root -> symbol) < 0)
		return search(root -> left, key);
    // go left
	else
        return search(root -> right, key);
}

/*------------------------------------------------------------------------------------*/
/*function : chk_mnemonic */
/*purpose : check that token is in optable or not */
/*return : sptr */
/*------------------------------------------------------------------------------------*/
int     chk_mnemonic(char* token)
{
    // check extended mode
    int flag = 0;
    if (token[0] == '+')
    {
        flag = 1;
        token = &token[1];
    }

    // hash of token
    int hash = ft_hash(token);
    // search optable
    for (hptr h = hash == -1 ? NULL : hash_table[hash]; h; h = h -> link)
    {
        // founded
        if (!strcmp(h -> mnemonic, token))
        {
            // if mnemonic return format
            return (flag + h -> fmt);
        }
    }
    // not found
    return (0);
}

/*------------------------------------------------------------------------------------*/
/*function : chk_mnemonic */
/*purpose : check which relative is right : base(2) or pc(4) */
/*return : int */
/*------------------------------------------------------------------------------------*/
int     chk_relative(int* displacement, int prog_counter, int base)
{
    // check pc relative
    if (*displacement - prog_counter >= -2048 && *displacement - prog_counter <= 2047) {
        // disp updated
        *displacement -= prog_counter; 
        // to get negative number
        *displacement &= 0xfff;
        return (2);
    }
    // check base relative
    else if (base != -1
                && *displacement - base >= 0 && *displacement - base <= 4095)
    {
        // displacement updated
        *displacement -= base;
        return (4);
    }
    else
        return (0);
}

/*------------------------------------------------------------------------------------*/
/*function : pass1 */
/*purpose : set the address to all line and make symbol table */
/*return : int */
/*------------------------------------------------------------------------------------*/
int     pass1(char* file)
{
    // start addr / locator
    int     start_addr = 0, loc = 0;
    // mnemonic format / END exist : 1
    int     format, end_flag=0;
    // obj file's start text record line, previous locator
    int     newline_loc, pre_loc = 0;
    // if RESB or RESW previously it will be 1
    int     pre_res_flag = 1;
    // if RESB or RESW currently it will be 1
    int     cur_res_flag;
    // instruction set
    char    *label, *mnemonic, *operand;
    // store a line of asm file
    char    line[MAX_LINE];

    // read file
    FILE *fp = fopen(file, "r");
    // store locctr of each line of asm file to immediate file
    FILE *tp = fopen("TEMP.txt", "w");
    // store start and end locctr of a text record line for obj file to intermediate file
    FILE *np = fopen("newLine.txt", "w");

    // error occurred : if there is no file it will active
    if (!fp)
    {
        printf("File Doesn't Exist!\n");
        return 0;
    }

    // start line
    fprintf(tp, "         \n%04X\n", start_addr);

    // read a line of asm file
    for (int line_index = 5; fgets(line, MAX_LINE, fp); line_index += 5)
    {
        // if label exists
        if ((line[0] >= 'A' && line[0] <= 'Z') || (line[0] >= '0' && line[0] <= '9'))
        {
            // get label and mnemonic
            label = strtok(line, LIMITER);
            mnemonic = strtok(NULL, LIMITER);
        }
        // if no label
        else
        {
            // label set to NULL and get mnemonic
            label = NULL;
            mnemonic = strtok(line, LIMITER);
        }
        // same as to get operand to here
        operand = strtok(NULL, LIMITER);
    
        //printf("pass1 : %d %s %s %s\n",line_index, label, mnemonic, operand);
        // mnemonic is comment lines or empty line
        if (!mnemonic || mnemonic[0] == '.') 
            continue;

        // mnemonic is NOBASE || BASE exist
        if (!strcmp(mnemonic, "NOBASE") || !strcmp(mnemonic, "BASE"))
            continue;

        // mnemonic is END
        if (!strcmp(mnemonic, "END"))
        {
            // set end flag since END exists
            end_flag = 1;
            break;
        }

        // mnemonic is START
        if (!strcmp(mnemonic, "START"))
        {
            // get start addr and init
            loc = hex_to_int(operand, strlen(operand) - 1);
            start_addr = loc;
            pre_loc = loc;
            // move file pointer to -5 point
            fseek(tp, -5, SEEK_CUR);
            // change start address
            fprintf(tp, "%04X\n", start_addr);
            continue;
        }

        // when it comes to label exists
        if (label)
        {
            // error
            //printf("error1?\n");
            if (chk_mnemonic(label) || !strcmp(label, "RESB") || !strcmp(label, "RESW")
                                || !strcmp(label, "BYTE") || !strcmp(label, "WORD"))
            {
                printf("Line %d : Reserved Word Can't be a Label!\n", line_index); 
                CLOSE
                return (0);
            }
            //printf("error2?\n");
            // error
            if (search(cur, label)) {
                printf("Line %d : Duplicate Symbol!\n", line_index);
                CLOSE
                return (0);
            }
            insert(&cur, loc, label);
        }

        cur_res_flag = 0;

        if (!strcmp(mnemonic, "BYTE")) {
            if (operand[0] == 'C')
                for (int i = 2; operand[i]!='\''; i++) loc += 1;
            else if (operand[0] == 'X')
                for (int i = 2; operand[i]!='\''; i += 2) loc += 1;
            else {
                printf("Line %d : Invaild Operand!\n", line_index);
                CLOSE
                return (0);
            }
        }
        // mnemonic is RESB
        else if (!strcmp(mnemonic, "RESB")) {
            loc += dec_to_int(operand, strlen(operand) - 1);
            cur_res_flag = 1;
        }
        // mnemonic is RESW
        else if (!strcmp(mnemonic, "RESW")) {
            loc += 3 * dec_to_int(operand, strlen(operand) - 1);
            cur_res_flag = 1;
        }
        // mnemonic is WORD
        else if (!strcmp(mnemonic, "WORD"))
            loc += 3;
        // mnemonic is not found : not exist
        else if (!(format = chk_mnemonic(mnemonic)))
        { 
            // error
            printf("Line %d : Invalid Operation Code!\n", line_index);
            CLOSE
            return 0;
        }
        // else
        else loc += format;

        // finish the text record line since RESB or RESW appear
        if (!pre_res_flag && cur_res_flag)
            fprintf(np, "%04X\n",  pre_loc);
        // change text record line since too long
        else if (!pre_res_flag && (loc - newline_loc) > 30)
            fprintf(np, "%04X\n%04X ", newline_loc = pre_loc, pre_loc);

        // start new text record line since switched RES to not
        if (pre_res_flag && !cur_res_flag)
            fprintf(np, "%04X ", newline_loc = pre_loc);

        fprintf(tp, "%04X\n", loc);
        pre_loc = loc;
        pre_res_flag = cur_res_flag;
    }
    // move tp to the temp.txt
    rewind(tp);
    // print start address and program size to file
    fprintf(tp, "%04X %04X\n", start_addr, (loc - start_addr));
    fprintf(np, "%04X\n", loc);
    CLOSE

    // if there is no END
    if (!end_flag)
    {
        printf("No END!\n");
        return (0);
    }
     // success
    return (1);
}
int     pass2(char* file)
{
    // size / start addr / mnemonic format / opcode
    int     size, start, format, opcode;
    // obj_s : start addr of a text record, obj_e : end addr of a text record
    int     obj_s, obj_e = -1;
    // if NOBASE base is -1 
    int     base = -1;
    // registers
    int     ni, x, bp, e, displacement; 

    // read line by line
    char    line[MAX_LINE];
    char    line_tmp[MAX_LINE];
     // instruction set
    char    *label, *mnemonic, *operand_1, *operand_2;

    // pointerss
    sptr    tmp;
    mptr    merc_rec = NULL;

    // open file to read
    FILE *fp = fopen(file, "r");
    // read intermediate files
    FILE *tp = fopen("TEMP.txt", "r"); 
    FILE *np = fopen("newLine.txt", "r");
    // change .asm extension to .lst
    strcpy(&file[strlen(file) - 3], "lst");
    // open lst file to write
    FILE *lp = fopen(file, "w");
    // change .lst extension to .obj
    strcpy(&file[strlen(file) - 3], "obj");
    // open obj file to write
    FILE *op = fopen(file, "w");

    // write program start addr and size
    fscanf(tp, "%X%X", &start, &size);
    fprintf(op, "H      %06X%06X\n", start, size);

    // start program line by line
    for (int line_index = 5; fgets(line, MAX_LINE, fp); line_index += 5)
    {
        //copy line to line_tmp and set line EOL
        strcpy(line_tmp, line);
        line[strlen(line) - 1] = '\0';

        // if label exists it will strtok label and mnemonic
        if ((line[0] >= 'A' && line[0] <= 'Z') || (line[0] >= '0' && line[0] <= '9') )
        {
            label = strtok(line_tmp, LIMITER);
            mnemonic = strtok(NULL, LIMITER);
        }
        // no label than strtok mnemonic only
        else {
            label = NULL;
            mnemonic = strtok(line_tmp, LIMITER);
        }

        //take operands
        operand_1 = strtok(NULL, LIMITER);
        operand_2 = strtok(NULL, LIMITER);
        //Just for error detect dont worry
        //printf("pass2: %d   %s %s %s %s\n",line_index, label, mnemonic, operand_1, operand_2);
        fprintf(lp, "%-8d", line_index);

        // comment line || empty line
        if (!mnemonic || mnemonic[0] == '.')
        {
            fprintf(lp, "        %s\n", line);
            continue;
        }
        
        // remove ',' betweem op1 and op2
        if (operand_1 && operand_2)
            operand_1[strlen(operand_1) - 1] = '\0';
        // set extended
        e = ((mnemonic[0] == '+') ? 1 : 0);
        // set indexing
        x = ((operand_2 && operand_2[0] == 'X') ? 1 : 0);

        // NOBASE
        if (!strcmp(mnemonic, "NOBASE"))
        {
            fprintf(lp, "        %s\n", line);
            continue;
        }

        // BASE
        if (!strcmp(mnemonic, "BASE"))
        {
            fprintf(lp, "        %s\n", line);
            // if operand is label
            if ((tmp = search(cur, operand_1)))
                base = tmp->address;
            // else
            else
                base = hex_to_int(operand_1, strlen(operand_1) - 1);
            continue;
        }

        // END
        if (!strcmp(mnemonic, "END"))
        {
            // print M record
            for (mptr w = merc_rec; w ; w = w -> link)
                fprintf(op, "\nM%06X05", w -> mrec);
            fprintf(op, "\nE%06X\n", obj_e - size);
            fprintf(lp, "        %s\n", line);
            break;
        }

        // START
        if (!strcmp(mnemonic, "START"))
        {
            fseek(op, -19 ,SEEK_CUR);
            // set the program name
            fprintf(op, "%-6s", label);
            fseek(op, 13 ,SEEK_CUR);
            fprintf(lp, "%04X    ", start);
            fprintf(lp, "%s\n", line);
            continue;
        }

        fscanf(tp, "%X", &start);
        // All new T record line started
        if (start >= obj_e)
        {
            // if it is not the first line
            if (obj_e != -1)
                fprintf(op, "\n");
            fscanf(np, "%04X %04X", &obj_s, &obj_e);
            fprintf(op, "T%06X%02X", obj_s, obj_e - obj_s);
        }
        fprintf(lp, "%04X    ", start);
        fprintf(lp, "%-28s", line);

        // WORD or BYTE
        if (!strcmp(mnemonic, "WORD") || !strcmp(mnemonic, "BYTE"))
        {
            // not a BYTE 
            if (mnemonic[0] != 'B')
            {
                fprintf(op, "%06X", dec_to_int(operand_1, strlen(operand_1) - 1));
                fprintf(lp, "%06X", dec_to_int(operand_1, strlen(operand_1)-1));
                fprintf(lp, "\n");
                continue;
            }

            // it's BYTE C''
            if (operand_1[0] == 'C')
            { 
                for (int i = 2; operand_1[i]!='\''; i += 1)
                {
                    fprintf(op, "%X", operand_1[i]);
                    fprintf(lp, "%X", operand_1[i]);
                }
                fprintf(lp, "\n");
                continue;
            }

            // it's BYTE X''
            for (int i = 2; operand_1[i]!='\''; i += 1)
            {
                fprintf(op, "%C", operand_1[i]);
                fprintf(lp, "%C", operand_1[i]);
            }
            fprintf(lp, "\n");
            continue;
        }

        // RESW or RESB
        if (!strcmp(mnemonic, "RESW") || !strcmp(mnemonic, "RESB"))
        {
            fprintf(lp, "\n");
            continue;
        }

        // search
        int hash = ft_hash(&mnemonic[e]);
        for (hptr w = hash_table[hash]; w; w = w->link)
        {
            // when it comes to found
            if (!strcmp(w->mnemonic, &mnemonic[e]))
            {
                format = w->fmt;
                opcode = hex_to_int(w->op, strlen(w->op) - 1);
                break;
            }
        }

        // format1
        if (format == 1) {
            printf("1\n");
            fprintf(op, "%02X", opcode);
            fprintf(lp, "%02X\n", opcode);
        }

        // format2
        else if (format == 2) {
            fprintf(op, "%02X", opcode);
            fprintf(lp, "%02X", opcode);
            fprintf(op, "%d", (operand_1 ? search(reg, operand_1) -> address : 0));
            fprintf(op, "%d", (operand_2 ? search(reg, operand_2) -> address : 0));
            fprintf(lp, "%d", (operand_1 ? search(reg, operand_1) -> address : 0));
            fprintf(lp, "%d\n", (operand_2 ? search(reg, operand_2) -> address : 0));
        }

        // format3
        else if (mnemonic[0] != '+')
        {
            //printf("3\n"); for test
            if (!operand_1)
            {
                fprintf(op, "%02X0000", opcode + 3);
                fprintf(lp, "%02X0000\n", opcode + 3);
                continue;
            }
            ni = ((operand_1[0] == '#') ? 1 : ((operand_1[0] == '@') ? 2 : 3));
            // set label
            if ((tmp = search(cur, &operand_1[ni != 3])))
                displacement = tmp->address;
            // if not label
            else 
                displacement = dec_to_int(&operand_1[ni != 3], strlen(operand_1) - 1 - (ni != 3));
            // if label then look for proper relative and compute disp
            bp = tmp ? chk_relative(&displacement, start + 3, base) : 0;

            if (tmp && !bp)
            {
                // error
                printf("Line %d : Address is Invalid!\n", line_index);
                CLOSE_ALL
                // remove the obj file
                remove(file);
                // change the .obj to extend .lst
                strcpy(&file[strlen(file)-3], "lst");
                // remove the lst file
                remove(file);
                return (0);
            }
            // write all the opcode, register
            fprintf(op, "%02X%01X%03X", opcode + ni, 8 * x + bp + e, displacement);
            fprintf(lp, "%02X%01X%03X\n", opcode + ni, 8 * x + bp + e, displacement);
        }
        
        // format4
        else {
            //printf("4\n"); for test
            if (!operand_1)
            {
                fprintf(op, "%02X000000", opcode + 3);
                fprintf(lp, "%02X000000\n", opcode + 3);
                continue;
            }

            ni = ((operand_1[0] == '#') ? 1 : (operand_1[0] == '@') ? 2 : 3);
            // store M record
            if ((tmp = search(cur, &operand_1[ni != 3])))
            {
                // make new NODE
                mptr n_mrec = (mptr)malloc(sizeof(NODE_MERC));
                n_mrec->mrec = start + 1;
                n_mrec->link = NULL;
                displacement = tmp->address;

                // if empty
                if (!merc_rec)
                    merc_rec = n_mrec;
                // else
                else
                {
                    for (mptr m = merc_rec; ; m = m->link)
                    {
                        if (!m -> link) {
                            m -> link = n_mrec;
                            break;
                        }
                    }
                }
            }
            else
                displacement = dec_to_int(&operand_1[ni != 3], strlen(operand_1) - 1 - (ni != 3));
            bp = 0;
            fprintf(op, "%02X%01X%05X", opcode + ni, 8 * x + bp + e, displacement);
            fprintf(lp, "%02X%01X%05X\n", opcode + ni, 8 * x + bp + e, displacement);
        }
    }
    CLOSE_ALL
    return (1);
}