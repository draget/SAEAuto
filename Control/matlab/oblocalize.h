/*
 * oblocalize.h
 *
 * Code generation for function 'oblocalize'
 *
 * C source code generated on: Fri Sep 26 14:14:02 2014
 *
 */

#ifndef __OBLOCALIZE_H__
#define __OBLOCALIZE_H__
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
extern void oblocalize(const emxArray_real_T *scoefx, const emxArray_real_T *scoefy, const emxArray_real_T *si, const emxArray_real_T *ob, real_T sguess, real_T epsilon, emxArray_real_T *arcob);
#endif
/* End of code generation (oblocalize.h) */
