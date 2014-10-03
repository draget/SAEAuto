/*
 * linspace.cpp
 *
 * Code generation for function 'linspace'
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
#include "linspace.h"
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
void linspace(real_T d1, real_T d2, real_T n1, emxArray_real_T *y)
{
  int32_T i8;
  real_T delta1;
  real_T delta2;
  int32_T k;
  if (n1 < 0.0) {
    n1 = 0.0;
  }

  i8 = y->size[0] * y->size[1];
  y->size[0] = 1;
  y->size[1] = (int32_T)n1;
  emxEnsureCapacity((emxArray__common *)y, i8, (int32_T)sizeof(real_T));
  if (y->size[1] >= 1) {
    y->data[y->size[1] - 1] = d2;
    if (y->size[1] >= 2) {
      y->data[0] = d1;
      if (y->size[1] >= 3) {
        if ((((int32_T)(d1 < 0.0) != (int32_T)(d2 < 0.0)) && (fabs(d1) >
              8.9884656743115785E+307)) || (fabs(d2) > 8.9884656743115785E+307))
        {
          delta1 = d1 / ((real_T)y->size[1] - 1.0);
          delta2 = d2 / ((real_T)y->size[1] - 1.0);
          i8 = y->size[1];
          for (k = 0; k <= i8 - 3; k++) {
            y->data[k + 1] = (d1 + delta2 * (1.0 + (real_T)k)) - delta1 * (1.0 +
              (real_T)k);
          }
        } else {
          delta1 = (d2 - d1) / ((real_T)y->size[1] - 1.0);
          i8 = y->size[1];
          for (k = 0; k <= i8 - 3; k++) {
            y->data[k + 1] = d1 + (1.0 + (real_T)k) * delta1;
          }
        }
      }
    }
  }
}

/* End of code generation (linspace.cpp) */
