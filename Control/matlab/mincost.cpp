/*
 * mincost.cpp
 *
 * Code generation for function 'mincost'
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
 * function [totalcosts,bestpath] = mincost(Ks, safetycost, Kpo, pathoffsetcost, Kc, conscost)
 */
void mincost(real_T Ks, const emxArray_real_T *safetycost, real_T Kpo, const
             emxArray_real_T *pathoffsetcost, real_T Kc, const emxArray_real_T
             *conscost, emxArray_real_T *totalcosts, real_T *bestpath)
{
  int32_T ixstart;
  int32_T n;
  real_T mtmp;
  int32_T itmp;
  int32_T ix;
  boolean_T exitg1;

  /* UNTITLED Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'mincost:5' totalcosts = Ks.*safetycost + Kpo.*pathoffsetcost + Kc.*conscost; */
  ixstart = totalcosts->size[0] * totalcosts->size[1];
  totalcosts->size[0] = 1;
  totalcosts->size[1] = safetycost->size[1];
  emxEnsureCapacity((emxArray__common *)totalcosts, ixstart, (int32_T)sizeof
                    (real_T));
  n = safetycost->size[0] * safetycost->size[1] - 1;
  for (ixstart = 0; ixstart <= n; ixstart++) {
    totalcosts->data[ixstart] = (Ks * safetycost->data[ixstart] + Kpo *
      pathoffsetcost->data[ixstart]) + Kc * conscost->data[ixstart];
  }

  /* 'mincost:7' [cost,bestpath] = min(totalcosts); */
  ixstart = 1;
  n = totalcosts->size[1];
  mtmp = totalcosts->data[0];
  itmp = 0;
  if (n > 1) {
    if (rtIsNaN(totalcosts->data[0])) {
      ix = 2;
      exitg1 = FALSE;
      while ((exitg1 == 0U) && (ix <= n)) {
        ixstart = ix;
        if (!rtIsNaN(totalcosts->data[ix - 1])) {
          mtmp = totalcosts->data[ix - 1];
          exitg1 = TRUE;
        } else {
          ix++;
        }
      }
    }

    if (ixstart < n) {
      while (ixstart + 1 <= n) {
        if (totalcosts->data[ixstart] < mtmp) {
          mtmp = totalcosts->data[ixstart];
          itmp = ixstart;
        }

        ixstart++;
      }
    }
  }

  *bestpath = (real_T)(itmp + 1);
}

/* End of code generation (mincost.cpp) */
