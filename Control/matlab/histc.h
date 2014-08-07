/*
 * histc.h
 *
 * Code generation for function 'histc'
 *
 * C source code generated on: Thu Aug  7 12:12:52 2014
 *
 */

#ifndef __HISTC_H__
#define __HISTC_H__
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
extern void b_histc(real_T X, const emxArray_real_T *edges, emxArray_real_T *N, real_T *BIN);
extern void c_histc(const real_T X[4], const emxArray_real_T *edges, emxArray_real_T *N, real_T BIN[4]);
extern void histc(const emxArray_real_T *X, const emxArray_real_T *edges, emxArray_real_T *N, emxArray_real_T *BIN);
#endif
/* End of code generation (histc.h) */
