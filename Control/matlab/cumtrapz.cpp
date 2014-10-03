/*
 * cumtrapz.cpp
 *
 * Code generation for function 'cumtrapz'
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
#include "cumtrapz.h"
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
void cumtrapz(const emxArray_real_T *x, const emxArray_real_T *y,
              emxArray_real_T *z)
{
  emxArray_real_T *b_x;
  int32_T j;
  int32_T iystart;
  int32_T dim;
  int32_T a;
  uint32_T sz[2];
  int32_T vstride;
  int32_T k;
  real_T s;
  int32_T iyz;
  real_T ylast;
  emxInit_real_T(&b_x, 1);
  j = b_x->size[0];
  b_x->size[0] = x->size[0];
  emxEnsureCapacity((emxArray__common *)b_x, j, (int32_T)sizeof(real_T));
  iystart = x->size[0] - 1;
  for (j = 0; j <= iystart; j++) {
    b_x->data[j] = x->data[j];
  }

  dim = 2;
  if (y->size[0] != 1) {
    dim = 1;
  }

  if (dim <= 1) {
    a = y->size[0];
  } else {
    a = 1;
  }

  for (iystart = 1; iystart <= a - 1; iystart++) {
    b_x->data[iystart - 1] = b_x->data[iystart] - b_x->data[iystart - 1];
  }

  if (y->size[0] == 0) {
    if (a == 0) {
      for (j = 0; j < 2; j++) {
        sz[j] = (uint32_T)j;
      }

      sz[dim - 1] = 1U;
      j = z->size[0] * z->size[1];
      z->size[0] = (int32_T)sz[0];
      z->size[1] = 1;
      emxEnsureCapacity((emxArray__common *)z, j, (int32_T)sizeof(real_T));
      iystart = (int32_T)sz[0] - 1;
      j = 0;
      while (j <= iystart) {
        z->data[0] = 0.0;
        j = 1;
      }
    } else {
      j = z->size[0] * z->size[1];
      z->size[0] = 0;
      z->size[1] = 1;
      emxEnsureCapacity((emxArray__common *)z, j, (int32_T)sizeof(real_T));
    }
  } else {
    sz[0] = (uint32_T)y->size[0];
    sz[1] = 1U;
    j = z->size[0] * z->size[1];
    z->size[0] = (int32_T)sz[0];
    z->size[1] = 1;
    emxEnsureCapacity((emxArray__common *)z, j, (int32_T)sizeof(real_T));
    vstride = 1;
    k = 1;
    while (k <= dim - 1) {
      vstride *= y->size[0];
      k = 2;
    }

    iystart = -1;
    for (j = 1; j <= vstride; j++) {
      iystart++;
      s = 0.0;
      dim = -1;
      iyz = iystart;
      ylast = y->data[iystart];
      z->data[iystart] = 0.0;
      for (k = 0; k <= a - 2; k++) {
        iyz += vstride;
        if (b_x->size[0] == 0) {
          ylast += y->data[iyz];
          s += ylast / 2.0;
        } else {
          dim++;
          ylast += y->data[iyz];
          s += b_x->data[dim] * (ylast / 2.0);
        }

        ylast = y->data[iyz];
        z->data[iyz] = s;
      }
    }
  }

  emxFree_real_T(&b_x);
}

/* End of code generation (cumtrapz.cpp) */
