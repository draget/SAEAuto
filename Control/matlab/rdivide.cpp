/*
 * rdivide.cpp
 *
 * Code generation for function 'rdivide'
 *
 * C source code generated on: Mon Sep  1 19:20:44 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "buildbfcurvature.h"
#include "builddetailedbf.h"
#include "buildmanouvers.h"
#include "checkpathcollision.h"
#include "equateoffsetcost.h"
#include "equatesafetycost.h"
#include "evalheading.h"
#include "localize.h"
#include "mincost.h"
#include "oblocalize.h"
#include "parevalspline.h"
#include "rdivide.h"
#include "matlab_emxutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 *
 */
void rdivide(const emxArray_real_T *x, const emxArray_real_T *y, emxArray_real_T
             *z)
{
  int32_T i3;
  int32_T loop_ub;
  i3 = z->size[0];
  z->size[0] = x->size[0];
  emxEnsureCapacity((emxArray__common *)z, i3, (int32_T)sizeof(real_T));
  loop_ub = x->size[0] - 1;
  for (i3 = 0; i3 <= loop_ub; i3++) {
    z->data[i3] = x->data[i3] / y->data[i3];
  }
}

/* End of code generation (rdivide.cpp) */
