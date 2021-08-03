#include "20182204.h"

/*------------------------------------------------------------------------------------*/
/*function : hex_to_int*/
/*purpose : change hex string to int */
/*return : ans
/*------------------------------------------------------------------------------------*/
int             hex_to_int(char *str, int index)
{
    int ans = 0;

    // change start string to integer
    for (int i = index, j = 1; i >= 0; j *= 16, i--)
    {
        if (str[i] >= '0' && str[i] <= '9')
            ans += (str[i] - '0') * j;
        else if (str[i] >= 'A' && str[i] <= 'F')
            ans += (str[i] - 'A' + 10) * j;
        else if (str[i] >= 'a' && str[i] <= 'f')
            ans += (str[i] - 'a' + 10) * j;
        // error : not a printable word 
        else
            return -1;
    }
    return (ans);
}

/* start code */
/*------------------------------------------------------------------------------------*/
/*function : main*/
/*purpose :  get input from user and check. if user's oreder is right it works as a mannual*/
/*return : 0
/*------------------------------------------------------------------------------------*/

int                 main(void)
{
    //user input
    char    ord[CMD_LEN];
    // copy user input
    char    ord_cpy[CMD_LEN];
    // last printed address
    int     cnt_dump = -1;

    while (1)
    {
        printf("sicsim> ");
        // standard user command input
        fgets(ord, CMD_LEN, stdin);

        // last word should be a null
        ord[strlen(ord) - 1] = 0;
        // copy user command and when it comes to strrok it change the string
        strcpy(ord_cpy, ord);

        // split the string by space
        char    *ord_ptr = strtok(ord_cpy, LIMITER);
        // if user didn't give any input back to while
        if (!ord_ptr) continue;
        // End Of String : it will check ord that ord has appropriate number of arguments
        char    *EOS;

        // d[ir]
        if (!strcmp(ord_ptr,"dir") || !strcmp(ord_ptr,"d"))
        { 
            if ((EOS = strtok(NULL, LIMITER))) //if not work
            { 
                printf("Too Many Arguments!\n");
                continue; 
            }
            dir();
            push(ord); // it will stored in histroy
        }
        // hi[story]
        else if (!strcmp(ord_ptr,"history") || !strcmp(ord_ptr,"hi"))
        { 
            if ((EOS = strtok(NULL, LIMITER))) //if not work
            {
                printf("Too Many Arguments!\n");
                continue; 
            }
            push(ord); // it will stored in histroy
            history();
        }
        // du[mp] || [start [end]]
        else if (!strcmp(ord_ptr, "dump") || !strcmp(ord_ptr, "du"))
        {
            // if exist start address it will get start addr
            char* start = strtok(NULL, LIMITER);
            // if exist end address it will get end addr
            char* end = strtok(NULL, LIMITER);

            if ((EOS = strtok(NULL, LIMITER))) //if not work
            {
                printf("Too Many Arguments!\n");
                continue; 
            }

            // save integer value in address
            int start_val = 0, end_val = 0;

            // du[mp]
            if (!start)
            {
                push(ord); // it will stored in histroy
                dump(cnt_dump+1, (cnt_dump+160 < MAX_MEMORY ? cnt_dump+160 : MAX_MEMORY)); // Fix Boundary
                cnt_dump = cnt_dump+160 < MAX_MEMORY ? cnt_dump+160 : -1; // Fix Boundary
                continue;
            }

            // error : if there's no ','
            if (end && start[strlen(start)-1] != ',') 
            { 
                printf("Invalid Arguments!\n");
                continue;
            }

            // if end string exist remove ',' from start string 
            start_val = hex_to_int(start, end != 0 ? strlen(start)-2 : strlen(start)-1);

            // error :  invalid hexadecimal number or invalid boundary
            if (start_val > MAX_MEMORY || start_val == -1) 
            {
                printf("Invalid Arguments!\n");
                continue;
            }

            //du[mp] start
            if (!end)
            {
                push(ord); // it will stored in histroy
                dump(start_val, (start_val+159 < MAX_MEMORY ? start_val+159 : MAX_MEMORY)); // Fix Boundary
                cnt_dump = start_val+159 < MAX_MEMORY ? start_val+159 : -1; // Fix Boundary
                continue;
            }

            end_val = hex_to_int(end, strlen(end)-1);

            // error :  invalid boundary or invalid hexadecimal number or start is bigger then end 
            if (end_val > MAX_MEMORY || end_val < start_val || end_val == -1)
            {
                printf("Invalid Arguments!\n");
                continue;
            }
            // du[mp] start end
            else
            {
                push(ord); // it will stored in histroy
                dump(start_val, end_val);
                cnt_dump = end_val != MAX_MEMORY ? end_val : -1; // Fix Boundary
            }
        }

        // e[dit]
        else if (!strcmp(ord_ptr, "edit") || !strcmp(ord_ptr, "e"))
        {
            char* addr = strtok(NULL, LIMITER);
            char* val = strtok(NULL, LIMITER);

            if ((EOS = strtok(NULL, LIMITER)))  //if not work
            {
                printf("Too Many Arguments!\n");
                continue; 
            }

            int addr_num = 0, val_num = 0;

            // error : missing requirement || no ','
            if (!addr || addr[strlen(addr)-1] != ',' || !val)
            {
                printf("Invalid Arguments!\n");
                continue;
            } 

            addr_num = hex_to_int(addr, strlen(addr)-2);
            val_num = hex_to_int(val, strlen(val)-1);

            // error : invalid hexa number or invalid boundary
            if (addr_num > MAX_MEMORY || addr_num == -1
                || val_num > MAX_VALUE || val_num == -1)
            {
                printf("Invalid Arguments!\n");
                continue;
            }

            push(ord); // it will stored in histroy
            edit(addr_num, val_num);
        }

        // f[ill]
        else if (!strcmp(ord_ptr, "fill") || !strcmp(ord_ptr, "f"))
        {
            int     start_val = 0;
            int     end_val = 0;
            int     val_num = 0;
            char*   start = strtok(NULL, LIMITER);
            char*   end = strtok(NULL, LIMITER);
            char*   val = strtok(NULL, LIMITER);

            if ((EOS = strtok(NULL, LIMITER))) //if not work
            {
                printf("Too Many Arguments!\n");
                continue; 
            }

            // error : missing requirement || no ','
            if (!start || start[strlen(start)-1] != ',' || !end
                || end[strlen(end)-1] != ',' || !val)
            {
                printf("Invalid Arguments!\n");
                continue;
            }

            start_val = hex_to_int(start, strlen(start)-2);
            end_val = hex_to_int(end, strlen(end)-2);
            val_num = hex_to_int(val, strlen(val)-1);

            // error : start is bigger then end or invalid boundary or invalid hexadecimal number
            if (start_val > MAX_MEMORY || start_val == -1 
            || end_val > MAX_MEMORY || end_val == -1 || end_val < start_val
            || val_num > MAX_VALUE || val_num == -1)
            { 
                printf("Invalid Arguments!\n");
                continue;
            }

            push(ord); // it will stored in histroy
            fill(start_val, end_val, val_num);
        }

        // reset
        else if (!strcmp(ord_ptr, "reset"))
        {
            if ((EOS = strtok(NULL, LIMITER))) //if not work
            {
                printf("Too Many Arguments!\n");
                continue; 
            }
            push(ord); // it will stored in histroy
            reset();
        }

        // opcode mnemonic
        else if (!strcmp(ord_ptr, "opcode"))
        {
            char*   MNEMONIC = strtok(NULL, LIMITER);
            if ((EOS = strtok(NULL, LIMITER))) //if not work
            {
                printf("Too Many Arguments!\n");
                continue; 
            }
            if (opcode(MNEMONIC))
            push(ord); // it will stored in histroy

        }
        // opcodelist
        else if (!strcmp(ord_ptr, "opcodelist"))
        {
            if ((EOS = strtok(NULL, LIMITER))) //if not work
            {
                printf("Too Many Arguments!\n");
                continue; 
            }
            if (opcodelist())
                push(ord); // it will stored in histroy
        }

        // q[uit]
        else if (!strcmp(ord_ptr,"quit") || !strcmp(ord_ptr,"q"))
        {
            if ((EOS = strtok(NULL, LIMITER))) //if not work
            {
                printf("Too Many Arguments!\n");
                continue; 
            }
            break; // stop
        }

        // h[elp]
        else if (!strcmp(ord_ptr, "help") || !strcmp(ord_ptr, "h"))
        {
            if((EOS = strtok(NULL, LIMITER))) //if not work
            {
                printf("Too Many Arguments!\n");
                continue;
            }
            help(); //display help;
            push(ord); // it will stored in histroy
        }else
        {
            printf("Invalid Command!\n");
        }
    }
    return (0);
}