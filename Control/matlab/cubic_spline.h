/*
 * cubic_spline.h
 *
 * Code generation for function 'cubic_spline'
 *
 * C source code generated on: Mon Sep  1 19:20:44 2014
 *
 */

#ifndef __CUBIC_SPLINE_H__
#define __CUBIC_SPLINE_H__
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
extern void cubic_spline(const emxArray_real_T *x, const emxArray_real_T *y, emxArray_real_T *s0, emxArray_real_T *s1, emxArray_real_T *s2, emxArray_real_T *s3);
#endif
/* End of code generation (cubic_spline.h) */
