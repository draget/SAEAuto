/*
 * buildbfcurvature.cpp
 *
 * Code generation for function 'buildbfcurvature'
 *
 * C source code generated on: Fri Sep 26 14:14:02 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "buildbfcurvature.h"
#include "builddetailedbf.h"
#include "buildmanouvers.h"
#include "checkpathcollision.h"
#include "equateconscost.h"
#include "equateoffsetcost.h"
#include "equatesafetycost.h"
#include "evalheading.h"
#include "genprevpathq.h"
#include "localize.h"
#include "mincost.h"
#include "oblocalize.h"
#include "parevalspline.h"
#include "matlab_emxutil.h"
#include "rdivide.h"
#include "power.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function k = buildbfcurvature(dxds, dyds, dx2ds, dy2ds)
 */
void buildbfcurvature(const emxArray_real_T *dxds, const emxArray_real_T *dyds,
                      const emxArray_real_T *dx2ds, const emxArray_real_T *dy2ds,
                      emxArray_real_T *k)
{
  emxArray_real_T *y;
  emxArray_real_T *a;
  int32_T i5;
  int32_T loop_ub;
  emxArray_real_T *b_dxds;
  emxInit_real_T(&y, 1);
  emxInit_real_T(&a, 1);

  /* UNTITLED Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'buildbfcurvature:5' k = ((dxds.*dy2ds)-(dx2ds.*dyds))./((dxds.^2+dyds.^2).^(3/2)); */
  power(dxds, a);
  power(dyds, y);
  i5 = a->size[0];
  a->size[0] = a->size[0];
  emxEnsureCapacity((emxArray__common *)a, i5, (int32_T)sizeof(real_T));
  loop_ub = a->size[0] - 1;
  for (i5 = 0; i5 <= loop_ub; i5++) {
    a->data[i5] += y->data[i5];
  }

  loop_ub = a->size[0];
  i5 = y->size[0];
  y->size[0] = loop_ub;
  emxEnsureCapacity((emxArray__common *)y, i5, (int32_T)sizeof(real_T));
  i5 = y->size[0];
  for (loop_ub = 0; loop_ub <= i5 - 1; loop_ub++) {
    y->data[loop_ub] = rt_powd_snf(a->data[loop_ub], 1.5);
  }

  emxFree_real_T(&a);
  emxInit_real_T(&b_dxds, 1);
  i5 = b_dxds->size[0];
  b_dxds->size[0] = dxds->size[0];
  emxEnsureCapacity((emxArray__common *)b_dxds, i5, (int32_T)sizeof(real_T));
  loop_ub = dxds->size[0] - 1;
  for (i5 = 0; i5 <= loop_ub; i5++) {
    b_dxds->data[i5] = dxds->data[i5] * dy2ds->data[i5] - dx2ds->data[i5] *
      dyds->data[i5];
  }

  rdivide(b_dxds, y, k);
  emxFree_real_T(&b_dxds);
  emxFree_real_T(&y);
}

/* End of code generation (buildbfcurvature.cpp) */
