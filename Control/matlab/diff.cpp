/*
 * diff.cpp
 *
 * Code generation for function 'diff'
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
#include "diff.h"
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
void diff(const emxArray_real_T *x, emxArray_real_T *y)
{
  int32_T ixStart;
  int32_T xNext;
  int32_T newDimSize;
  uint32_T ySize[2];
  emxArray_real_T *b_y1;
  int32_T iyStart;
  int32_T r;
  int32_T ixLead;
  int32_T iyLead;
  real_T work_data_idx_0;
  int32_T m;
  real_T tmp1;
  real_T tmp2;
  if (x->size[0] == 0) {
    ixStart = y->size[0] * y->size[1];
    y->size[0] = 0;
    y->size[1] = 2;
    emxEnsureCapacity((emxArray__common *)y, ixStart, (int32_T)sizeof(real_T));
  } else {
    xNext = x->size[0] - 1;
    if (xNext <= 1) {
    } else {
      xNext = 1;
    }

    if (xNext < 1) {
      ixStart = y->size[0] * y->size[1];
      y->size[0] = 0;
      y->size[1] = 0;
      emxEnsureCapacity((emxArray__common *)y, ixStart, (int32_T)sizeof(real_T));
    } else {
      newDimSize = x->size[0] - 1;
      for (ixStart = 0; ixStart < 2; ixStart++) {
        ySize[ixStart] = (uint32_T)x->size[ixStart];
      }

      b_emxInit_real_T(&b_y1, 2);
      ySize[0] = (uint32_T)newDimSize;
      ixStart = b_y1->size[0] * b_y1->size[1];
      b_y1->size[0] = (int32_T)ySize[0];
      b_y1->size[1] = 2;
      emxEnsureCapacity((emxArray__common *)b_y1, ixStart, (int32_T)sizeof
                        (real_T));
      xNext = x->size[0];
      ixStart = 1;
      iyStart = 1;
      for (r = 0; r < 2; r++) {
        ixLead = ixStart;
        iyLead = iyStart;
        work_data_idx_0 = x->data[ixStart - 1];
        for (m = 2; m <= x->size[0]; m++) {
          tmp1 = x->data[ixLead];
          tmp2 = work_data_idx_0;
          work_data_idx_0 = tmp1;
          tmp1 -= tmp2;
          ixLead++;
          b_y1->data[iyLead - 1] = tmp1;
          iyLead++;
        }

        ixStart += xNext;
        iyStart += newDimSize;
      }

      ixStart = y->size[0] * y->size[1];
      y->size[0] = b_y1->size[0];
      y->size[1] = 2;
      emxEnsureCapacity((emxArray__common *)y, ixStart, (int32_T)sizeof(real_T));
      xNext = b_y1->size[0] * b_y1->size[1] - 1;
      for (ixStart = 0; ixStart <= xNext; ixStart++) {
        y->data[ixStart] = b_y1->data[ixStart];
      }

      emxFree_real_T(&b_y1);
    }
  }
}

/* End of code generation (diff.cpp) */
