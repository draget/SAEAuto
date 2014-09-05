/*
 * mincost.h
 *
 * Code generation for function 'mincost'
 *
 * C source code generated on: Mon Sep  1 19:20:44 2014
 *
 */

#ifndef __MINCOST_H__
#define __MINCOST_H__
/* Include files */
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"

#include "rtwtypes.h"
#include "matlab_types.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
extern void mincost(real_T Ks, const emxArray_real_T *safetycost, real_T Kpo, const emxArray_real_T *pathoffsetcost, emxArray_real_T *totalcosts, real_T *bestpath);
#endif
/* End of code generation (mincost.h) */
