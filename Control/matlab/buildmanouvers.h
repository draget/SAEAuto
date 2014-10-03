/*
 * buildmanouvers.h
 *
 * Code generation for function 'buildmanouvers'
 *
 * C source code generated on: Fri Sep 26 14:14:02 2014
 *
 */

#ifndef __BUILDMANOUVERS_H__
#define __BUILDMANOUVERS_H__
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
extern void buildmanouvers(real_T scp, real_T cpdistance, real_T px, real_T py, real_T maxlatoffset, real_T mangran, real_T manlength, real_T mincurverad, real_T thetadiff, const emxArray_real_T *ss, real_T sindex, const emxArray_real_T *kk, real_T paththeta, emxArray_real_T *mxi, emxArray_real_T *myi, emxArray_real_T *pathki, emxArray_real_T *pathqi, emxArray_real_T *dthetai, emxArray_real_T *mans);
#endif
/* End of code generation (buildmanouvers.h) */
