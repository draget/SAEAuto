/*
 * ismember.cpp
 *
 * Code generation for function 'ismember'
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
#include "ismember.h"
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
void ismember(const emxArray_real_T *a, const emxArray_real_T *s,
              emxArray_boolean_T *tf)
{
  int32_T na;
  uint32_T ilo;
  int32_T k;
  int32_T ns;
  int32_T n;
  uint32_T ihi;
  boolean_T exitg2;
  uint32_T imid;
  real_T absxk;
  int32_T exponent;
  boolean_T p;
  boolean_T exitg1;
  int32_T b_exponent;
  na = a->size[0];
  ilo = (uint32_T)a->size[0];
  k = tf->size[0];
  tf->size[0] = (int32_T)ilo;
  emxEnsureCapacity((emxArray__common *)tf, k, (int32_T)sizeof(boolean_T));
  ns = (int32_T)ilo - 1;
  for (k = 0; k <= ns; k++) {
    tf->data[k] = FALSE;
  }

  for (k = 0; k + 1 <= na; k++) {
    ns = s->size[0];
    n = 0;
    ilo = 1U;
    ihi = (uint32_T)ns;
    exitg2 = FALSE;
    while ((exitg2 == 0U) && (ihi >= ilo)) {
      imid = (ilo >> 1U) + (ihi >> 1U);
      if (((ilo & 1U) == 1U) && ((ihi & 1U) == 1U)) {
        imid++;
      }

      absxk = fabs(s->data[(int32_T)imid - 1] / 2.0);
      if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
        if (absxk <= 2.2250738585072014E-308) {
          absxk = 4.94065645841247E-324;
        } else {
          frexp(absxk, &exponent);
          absxk = ldexp(1.0, exponent - 53);
        }
      } else {
        absxk = rtNaN;
      }

      if ((fabs(s->data[(int32_T)imid - 1] - a->data[k]) < absxk) || (rtIsInf
           (a->data[k]) && rtIsInf(s->data[(int32_T)imid - 1]) && ((int32_T)
            (a->data[k] > 0.0) == (int32_T)(s->data[(int32_T)imid - 1] > 0.0))))
      {
        p = TRUE;
      } else {
        p = FALSE;
      }

      if (p) {
        n = (int32_T)imid;
        exitg2 = TRUE;
      } else {
        if ((a->data[k] < s->data[(int32_T)imid - 1]) || rtIsNaN(s->data
             [(int32_T)imid - 1])) {
          p = TRUE;
        } else {
          p = FALSE;
        }

        if (p) {
          ihi = imid - 1U;
        } else {
          ilo = imid + 1U;
        }
      }
    }

    if (n > 0) {
      exitg1 = FALSE;
      while ((exitg1 == 0U) && (n + 1 <= ns)) {
        absxk = fabs(s->data[n] / 2.0);
        if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
          if (absxk <= 2.2250738585072014E-308) {
            absxk = 4.94065645841247E-324;
          } else {
            frexp(absxk, &b_exponent);
            absxk = ldexp(1.0, b_exponent - 53);
          }
        } else {
          absxk = rtNaN;
        }

        if ((fabs(s->data[n] - a->data[k]) < absxk) || (rtIsInf(a->data[k]) &&
             rtIsInf(s->data[n]) && ((int32_T)(a->data[k] > 0.0) == (int32_T)
              (s->data[n] > 0.0)))) {
          p = TRUE;
        } else {
          p = FALSE;
        }

        if (p) {
          n++;
        } else {
          exitg1 = TRUE;
        }
      }
    }

    if (n > 0) {
      tf->data[k] = TRUE;
    }
  }
}

/* End of code generation (ismember.cpp) */
