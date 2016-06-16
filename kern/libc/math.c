#include <math.h>

#if defined(i386) || defined(i486) || \
	defined(intel) || defined(x86) || defined(i86pc) || \
	defined(__alpha) || defined(__osf__)
#define __LITTLE_ENDIAN
#endif

#ifdef __LITTLE_ENDIAN
#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#define __HIp(x) *(1+(int*)x)
#define __LOp(x) *(int*)x
#else
#define __HI(x) *(int*)&x
#define __LO(x) *(1+(int*)&x)
#define __HIp(x) *(int*)x
#define __LOp(x) *(1+(int*)x)
#endif

double sin (double x)
{
  double x2 = x * x;
  return ((((.00015148419 * x2
             - .00467376557) * x2
            + .07968967928) * x2
           - .64596371106) * x2
          + 1.57079631847) * x;
}

double cos(double x)
{
  return sin(x + PI/2);
}

static const double huge = 1.0e300;

/*double floor(double x)
{
	int i0, i1, j0;
	unsigned i, j;
	i0 = __HI(x);
	i1 = __LO(x);
	j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
	if(j0 < 20)
	{
		if(j0 < 0)
		{
			if(huge + x > 0.0)
			{
				if(i0 >= 0)
				{
					i0 = i1 = 0;
				}
				else if(((i0&0x7fffffff) | i1) != 0)
				{
					i0 = 0xbff00000;
					i1 = 0;
				}
			}
		}
		else
		{
			i = (0x000fffff) >> j0;
			if(((i0 & i) | i1) == 0) return x;
			if(huge + x > 0.0)
			{
				if(i0 < 0) i0 += (0x00100000) >> j0;
				i0 &= (~i); i1 = 0;
			}
		}
	}
	else if (j0 > 51)
	{
		if(j0 == 0x400) return x+x;
		else return x;
	}
	else
	{
		i = ((unsigned)(0xffffffff)) >> (j0 - 20);
		if((i1 & 1) == 0) return x;
		if(huge + x > 0.0)
		{
			if(i0 < 0)
			{
				if(j0 == 20) i0 += 1;
				else
				{
					j = i1 + (1 << (52 - j0));
					if(j < i1)
						i0 += 1;
					i1 = j;
				}
			}
			i1 &= (~i);
		}
	}
	__HI(x) = i0;
	__LO(x) = i1;
	return x;
}*/

double floor(double x)
{
	if(x>0)return (int)x;
	return (int)(x-0.9999999999999999);
}

double pow(double x, int y) { 
    double z = 1.0;

    while (y > 0) {
        while (!(y&1)) {
            y >>= 2;
            x *= x;
        }
        --y;
        z = x * z;
    }
    return z;
}

double abs(double a)
{
	return (a >= 0) ? a : -a;
}

double sqrt(const double n)
{
	double a = 0.001;
	double l, u, g;

	if(n < 1)
	{
		l = n;
		u = 1;
	}
	else
	{
		l = 1;
		u = n;
	}

	while((u - l) > a)
	{
		g = (l + u) / 2;
		if(g * g > n)
			u = g;
		else
			l = g;
	}
	return (l+u)/2;
}