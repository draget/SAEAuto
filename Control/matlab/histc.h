/*
 * histc.h
 *
 * Code generation for function 'histc'
 *
 * C source code generated on: Fri Sep 26 14:14:02 2014
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
extern void histc(const emxArray_real_T *X, const emxArray_real_T *edges, emxArray_real_T *N, emxArray_real_T *BIN);
#endif
/* End of code generation (histc.h) */
