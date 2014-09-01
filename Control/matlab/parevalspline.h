/*
 * parevalspline.h
 *
 * Code generation for function 'parevalspline'
 *
 * C source code generated on: Mon Sep  1 19:20:44 2014
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
extern void b_parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks, const emxArray_real_T *t, emxArray_real_T *result);
extern void c_parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks, real_T t, real_T *result, real_T *curvn);
extern real_T d_parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks, real_T t);
extern void parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks, const emxArray_real_T *t, real_T d, emxArray_real_T *result, emxArray_real_T *curvn);
#endif
/* End of code generation (parevalspline.h) */
