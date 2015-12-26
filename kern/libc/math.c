#include <math.h>

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
