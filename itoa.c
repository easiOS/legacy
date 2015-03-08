#include "itoa.h"

/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
    /*int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap(*(str+start), *(str+end));
        start++;
        end--;
    }*/
}
 
// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    if (num == 0)
    {
        char zero[1];
        zero[0] = '0';
        str = zero;
        return zero;
    }

    int32_t acc = num;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    str = c2;
    return c2;
}