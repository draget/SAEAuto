/*
 * mldivide.h
 *
 * Code generation for function 'mldivide'
 *
 * C source code generated on: Fri Sep 26 14:14:02 2014
 *
 */

#ifndef __MLDIVIDE_H__
#define __MLDIVIDE_H__
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
extern void b_mldivide(const emxArray_real_T *A, emxArray_real_T *B);
extern void mldivide(const real_T A[4], const real_T B[2], real_T Y[2]);
#endif
/* End of code generation (mldivide.h) */
