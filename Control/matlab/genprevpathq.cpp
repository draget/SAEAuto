/*
 * genprevpathq.cpp
 *
 * Code generation for function 'genprevpathq'
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
 * function previouspathq = genprevpathq(bestpath, pathqi, mans)
 */
void genprevpathq(real_T bestpath, const emxArray_real_T *pathqi, const
                  emxArray_real_T *mans, emxArray_real_T *previouspathq)
{
  emxArray_int32_T *r27;
  int32_T i16;
  int32_T mans_idx_0;
  int32_T unnamed_idx_0;
  int32_T loop_ub;
  emxArray_real_T *b_pathqi;
  emxArray_real_T *b_mans;
  int32_T b_mans_idx_0;
  b_emxInit_int32_T(&r27, 1);

  /* UNTITLED2 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'genprevpathq:5' previouspathq = [pathqi(:,bestpath) mans]; */
  i16 = pathqi->size[0];
  mans_idx_0 = pathqi->size[0];
  unnamed_idx_0 = r27->size[0];
  r27->size[0] = mans_idx_0;
  emxEnsureCapacity((emxArray__common *)r27, unnamed_idx_0, (int32_T)sizeof
                    (int32_T));
  loop_ub = mans_idx_0 - 1;
  for (mans_idx_0 = 0; mans_idx_0 <= loop_ub; mans_idx_0++) {
    r27->data[mans_idx_0] = 1 + mans_idx_0;
  }

  emxInit_real_T(&b_pathqi, 1);
  mans_idx_0 = b_pathqi->size[0];
  b_pathqi->size[0] = i16;
  emxEnsureCapacity((emxArray__common *)b_pathqi, mans_idx_0, (int32_T)sizeof
                    (real_T));
  loop_ub = i16 - 1;
  for (i16 = 0; i16 <= loop_ub; i16++) {
    b_pathqi->data[i16] = pathqi->data[i16 + pathqi->size[0] * ((int32_T)
      bestpath - 1)];
  }

  b_emxInit_real_T(&b_mans, 2);
  unnamed_idx_0 = r27->size[0];
  mans_idx_0 = mans->size[0];
  b_mans_idx_0 = mans->size[0];
  i16 = b_mans->size[0] * b_mans->size[1];
  b_mans->size[0] = mans_idx_0;
  b_mans->size[1] = 1;
  emxEnsureCapacity((emxArray__common *)b_mans, i16, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r27);
  i16 = 0;
  while (i16 <= 0) {
    loop_ub = mans_idx_0 - 1;
    for (i16 = 0; i16 <= loop_ub; i16++) {
      b_mans->data[i16] = mans->data[i16];
    }

    i16 = 1;
  }

  i16 = previouspathq->size[0] * previouspathq->size[1];
  previouspathq->size[0] = unnamed_idx_0;
  previouspathq->size[1] = 2;
  emxEnsureCapacity((emxArray__common *)previouspathq, i16, (int32_T)sizeof
                    (real_T));
  i16 = 0;
  while (i16 <= 0) {
    loop_ub = unnamed_idx_0 - 1;
    for (i16 = 0; i16 <= loop_ub; i16++) {
      previouspathq->data[i16] = b_pathqi->data[i16];
    }

    i16 = 1;
  }

  emxFree_real_T(&b_pathqi);
  loop_ub = b_mans_idx_0 - 1;
  for (i16 = 0; i16 <= loop_ub; i16++) {
    previouspathq->data[i16 + previouspathq->size[0]] = b_mans->data[i16];
  }

  emxFree_real_T(&b_mans);
}

/* End of code generation (genprevpathq.cpp) */
