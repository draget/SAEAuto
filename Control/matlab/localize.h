/*
 * localize.h
 *
 * Code generation for function 'localize'
 *
 * C source code generated on: Fri Sep 26 14:14:02 2014
 *
 */

#ifndef __LOCALIZE_H__
#define __LOCALIZE_H__
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
extern void localize(const emxArray_real_T *coefx, const emxArray_real_T *coefy, const emxArray_real_T *breaks, real_T x0, real_T b_y0, real_T sguess, real_T epsilon, real_T *value, real_T *distance, real_T *loccurvn, real_T *count);
#endif
/* End of code generation (localize.h) */
