#include "ch_utility.h"

/*------------------------------------------------------------------------------------*/
/*function : hex_to_int*/
/*purpose : change hex string to int */
/*return : ans */
/*------------------------------------------------------------------------------------*/
int     hex_to_int(char *hex, int index)
{
    int ans = 0;

    // change start string to integer
    for (int i = index, j = 1; i >= 0; j *= 16, i--)
    {
        if (hex[i] >= '0' && hex[i] <= '9')
            ans += (hex[i] - '0') * j;
        else if (hex[i] >= 'A' && hex[i] <= 'F')
            ans += (hex[i] - 'A' + 10) * j;
        else if (hex[i] >= 'a' && hex[i] <= 'f')
            ans += (hex[i] - 'a' + 10) * j;
        // error : not a printable word 
        else
            return -1;
    }
    return (ans);
}

/*------------------------------------------------------------------------------------*/
/*function : dec_to_int*/
/*purpose : change decimal string to int */
/*return : ans */
/*------------------------------------------------------------------------------------*/
int     dec_to_int(char *dec, int index)
{
    int ans = 0;
    for(int i = index, j = 1; i >= 0; i--, j *= 10)
    {
        if(dec[i] >= '0' && dec[i] <= '9')
            ans += (dec[i] - '0') * j;
        //error case
        else
            return (-1);
    }
    return (ans);
}