/*
 * equateoffsetcost.cpp
 *
 * Code generation for function 'equateoffsetcost'
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

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function offsetcost = equateoffsetcost(pathqi)
 */
void equateoffsetcost(const emxArray_real_T *pathqi, emxArray_real_T *offsetcost)
{
  emxArray_real_T *b_pathqi;
  int32_T n;
  int32_T c_pathqi;
  int32_T ixstart;
  uint32_T uv0[2];
  int32_T k;
  emxArray_int32_T *r26;
  int32_T exitg3;
  real_T mtmp;
  real_T b_mtmp;
  boolean_T exitg2;
  boolean_T exitg1;
  b_emxInit_real_T(&b_pathqi, 2);

  /* UNTITLED2 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'equateoffsetcost:6' pathoffsetcost = abs(pathqi(end,:)) */
  n = pathqi->size[1];
  c_pathqi = pathqi->size[0];
  ixstart = b_pathqi->size[0] * b_pathqi->size[1];
  b_pathqi->size[0] = 1;
  b_pathqi->size[1] = n;
  emxEnsureCapacity((emxArray__common *)b_pathqi, ixstart, (int32_T)sizeof
                    (real_T));
  ixstart = n - 1;
  for (n = 0; n <= ixstart; n++) {
    b_pathqi->data[b_pathqi->size[0] * n] = pathqi->data[(c_pathqi +
      pathqi->size[0] * n) - 1];
  }

  for (n = 0; n < 2; n++) {
    uv0[n] = (uint32_T)b_pathqi->size[n];
  }

  emxFree_real_T(&b_pathqi);
  n = offsetcost->size[0] * offsetcost->size[1];
  offsetcost->size[0] = 1;
  offsetcost->size[1] = (int32_T)uv0[1];
  emxEnsureCapacity((emxArray__common *)offsetcost, n, (int32_T)sizeof(real_T));
  k = 0;
  b_emxInit_int32_T(&r26, 1);
  do {
    exitg3 = 0;
    n = pathqi->size[1];
    ixstart = r26->size[0];
    r26->size[0] = n;
    emxEnsureCapacity((emxArray__common *)r26, ixstart, (int32_T)sizeof(int32_T));
    ixstart = n - 1;
    for (n = 0; n <= ixstart; n++) {
      r26->data[n] = 1 + n;
    }

    if (k <= r26->size[0] - 1) {
      c_pathqi = pathqi->size[0];
      mtmp = pathqi->data[(c_pathqi + pathqi->size[0] * k) - 1];
      offsetcost->data[k] = fabs(mtmp);
      k++;
    } else {
      exitg3 = 1;
    }
  } while (exitg3 == 0U);

  emxFree_int32_T(&r26);

  /* 'equateoffsetcost:8' minoffsetcost = min(pathoffsetcost); */
  ixstart = 1;
  n = offsetcost->size[1];
  b_mtmp = offsetcost->data[0];
  if (n > 1) {
    if (rtIsNaN(offsetcost->data[0])) {
      c_pathqi = 2;
      exitg2 = FALSE;
      while ((exitg2 == 0U) && (c_pathqi <= n)) {
        ixstart = c_pathqi;
        if (!rtIsNaN(offsetcost->data[c_pathqi - 1])) {
          b_mtmp = offsetcost->data[c_pathqi - 1];
          exitg2 = TRUE;
        } else {
          c_pathqi++;
        }
      }
    }

    if (ixstart < n) {
      while (ixstart + 1 <= n) {
        if (offsetcost->data[ixstart] < b_mtmp) {
          b_mtmp = offsetcost->data[ixstart];
        }

        ixstart++;
      }
    }
  }

  /* 'equateoffsetcost:9' maxoffsetcost = max(pathoffsetcost); */
  ixstart = 1;
  n = offsetcost->size[1];
  mtmp = offsetcost->data[0];
  if (n > 1) {
    if (rtIsNaN(offsetcost->data[0])) {
      c_pathqi = 2;
      exitg1 = FALSE;
      while ((exitg1 == 0U) && (c_pathqi <= n)) {
        ixstart = c_pathqi;
        if (!rtIsNaN(offsetcost->data[c_pathqi - 1])) {
          mtmp = offsetcost->data[c_pathqi - 1];
          exitg1 = TRUE;
        } else {
          c_pathqi++;
        }
      }
    }

    if (ixstart < n) {
      while (ixstart + 1 <= n) {
        if (offsetcost->data[ixstart] > mtmp) {
          mtmp = offsetcost->data[ixstart];
        }

        ixstart++;
      }
    }
  }

  /* 'equateoffsetcost:10' offsetcost = (pathoffsetcost-minoffsetcost)*(1/(maxoffsetcost - minoffsetcost)); */
  mtmp = 1.0 / (mtmp - b_mtmp);
  n = offsetcost->size[0] * offsetcost->size[1];
  offsetcost->size[0] = 1;
  offsetcost->size[1] = offsetcost->size[1];
  emxEnsureCapacity((emxArray__common *)offsetcost, n, (int32_T)sizeof(real_T));
  ixstart = offsetcost->size[0];
  n = offsetcost->size[1];
  ixstart = ixstart * n - 1;
  for (n = 0; n <= ixstart; n++) {
    offsetcost->data[n] = (offsetcost->data[n] - b_mtmp) * mtmp;
  }
}

/* End of code generation (equateoffsetcost.cpp) */
