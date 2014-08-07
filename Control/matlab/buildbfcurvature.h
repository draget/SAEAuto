/*
 * buildbfcurvature.h
 *
 * Code generation for function 'buildbfcurvature'
 *
 * C source code generated on: Thu Aug  7 12:12:52 2014
 *
 */

#ifndef __BUILDBFCURVATURE_H__
#define __BUILDBFCURVATURE_H__
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
extern void buildbfcurvature(const emxArray_real_T *dxds, const emxArray_real_T *dyds, const emxArray_real_T *dx2ds, const emxArray_real_T *dy2ds, emxArray_real_T *k);
#endif
/* End of code generation (buildbfcurvature.h) */
