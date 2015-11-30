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
        str[0] = '0';
        str[1] = '\0';
        return str;
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
    for(int j = 0; j < 32; j++)
    {
      if(c2[j] == '\0') break;
      str[j] = c2[j];
    }
    return str;
}

int isdigit (char c) {
    return (c>='0') && (c<='9');
}

long long int atoi(const char *c)
{
    long long int value = 0;
    int sign = 1;
    if( *c == '+' || *c == '-' )
    {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while (isdigit(*c))
    {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return (value * sign);
}
