/*
 * parevalspline.h
 *
 * Code generation for function 'parevalspline'
 *
 * C source code generated on: Wed Jul 23 22:33:34 2014
 *
 */

#ifndef __PAREVALSPLINE_H__
#define __PAREVALSPLINE_H__
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
extern void parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks, const emxArray_real_T *t, emxArray_real_T *result);
#endif
/* End of code generation (parevalspline.h) */
