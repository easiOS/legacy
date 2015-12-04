#include "itoa.h"

/* A utility function to reverse a string  */
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

// Implementation of itoa()
/*char* itoa(uint32_t num, char* str, int base)
{
    if (num == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
    if(base == 10)
    {
      uint32_t acc = num;
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
    }
    if(base == 2)
    {
      char c[32];
      for(int i = 0; i < 32; i++)
      {
        c[31-i] = ((num>>i) & 1) ? '1' : '0';
      }
      for(int j = 0; j < 32; j++)
      {
        if(c[j] == '\0') break;
        str[j] = c[j];
      }
    }
    return str;
}*/

//K&R implementation

void itoa(int64_t n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
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
