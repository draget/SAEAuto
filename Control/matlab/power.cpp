/*
 * power.cpp
 *
 * Code generation for function 'power'
 *
 * C source code generated on: Thu Aug  7 12:12:52 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "buildbfcurvature.h"
#include "builddetailedbf.h"
#include "localize.h"
#include "parevalspline.h"
#include "power.h"
#include "matlab_emxutil.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 *
 */
void b_power(const emxArray_real_T *a, emxArray_real_T *y)
{
  uint32_T unnamed_idx_0;
  int32_T i8;
  int32_T k;
  unnamed_idx_0 = (uint32_T)a->size[0];
  i8 = y->size[0];
  y->size[0] = (int32_T)unnamed_idx_0;
  emxEnsureCapacity((emxArray__common *)y, i8, (int32_T)sizeof(real_T));
  i8 = y->size[0];
  for (k = 0; k <= i8 - 1; k++) {
    y->data[k] = rt_powd_snf(a->data[k], 3.0);
  }
}

/*
 *
 */
void power(const emxArray_real_T *a, emxArray_real_T *y)
{
  uint32_T unnamed_idx_0;
  int32_T i5;
  int32_T k;
  unnamed_idx_0 = (uint32_T)a->size[0];
  i5 = y->size[0];
  y->size[0] = (int32_T)unnamed_idx_0;
  emxEnsureCapacity((emxArray__common *)y, i5, (int32_T)sizeof(real_T));
  i5 = y->size[0];
  for (k = 0; k <= i5 - 1; k++) {
    y->data[k] = rt_powd_snf(a->data[k], 2.0);
  }
}

/* End of code generation (power.cpp) */
