/*
 * builddetailedbf.cpp
 *
 * Code generation for function 'builddetailedbf'
 *
 * C source code generated on: Wed Jul 23 22:33:34 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "builddetailedbf.h"
#include "parevalspline.h"
#include "matlab_emxutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function [sx,sy,ss] = builddetailedbf(scoefx,scoefy,si,gran)
 */
void builddetailedbf(const emxArray_real_T *scoefx, const emxArray_real_T
                     *scoefy, const emxArray_real_T *si, real_T gran,
                     emxArray_real_T *sx, emxArray_real_T *sy, emxArray_real_T
                     *ss)
{
  real_T distance;
  emxArray_real_T *b_ss;
  int32_T i4;
  real_T delta2;
  int32_T k;

  /* 'builddetailedbf:3' distance = si(end) - si(1); */
  distance = si->data[si->size[0] - 1] - si->data[0];

  /* 'builddetailedbf:4' npoints = distance/gran; */
  distance /= gran;

  /* 'builddetailedbf:6' ss = linspace(si(1),si(end),npoints); */
  if (distance < 0.0) {
    distance = 0.0;
  }

  b_emxInit_real_T(&b_ss, 2);
  i4 = b_ss->size[0] * b_ss->size[1];
  b_ss->size[0] = 1;
  b_ss->size[1] = (int32_T)floor(distance);
  emxEnsureCapacity((emxArray__common *)b_ss, i4, (int32_T)sizeof(real_T));
  if (b_ss->size[1] >= 1) {
    b_ss->data[b_ss->size[1] - 1] = si->data[si->size[0] - 1];
    if (b_ss->size[1] >= 2) {
      b_ss->data[0] = si->data[0];
      if (b_ss->size[1] >= 3) {
        if ((((int32_T)(si->data[0] < 0.0) != (int32_T)(si->data[si->size[0] - 1]
               < 0.0)) && (fabs(si->data[0]) > 8.9884656743115785E+307)) ||
            (fabs(si->data[si->size[0] - 1]) > 8.9884656743115785E+307)) {
          distance = si->data[0] / ((real_T)b_ss->size[1] - 1.0);
          delta2 = si->data[si->size[0] - 1] / ((real_T)b_ss->size[1] - 1.0);
          i4 = b_ss->size[1];
          for (k = 0; k <= i4 - 3; k++) {
            b_ss->data[k + 1] = (si->data[0] + delta2 * (1.0 + (real_T)k)) -
              distance * (1.0 + (real_T)k);
          }
        } else {
          distance = (si->data[si->size[0] - 1] - si->data[0]) / ((real_T)
            b_ss->size[1] - 1.0);
          i4 = b_ss->size[1];
          for (k = 0; k <= i4 - 3; k++) {
            b_ss->data[k + 1] = si->data[0] + (1.0 + (real_T)k) * distance;
          }
        }
      }
    }
  }

  /* 'builddetailedbf:8' ss = ss'; */
  i4 = ss->size[0];
  ss->size[0] = b_ss->size[1];
  emxEnsureCapacity((emxArray__common *)ss, i4, (int32_T)sizeof(real_T));
  k = b_ss->size[1] - 1;
  for (i4 = 0; i4 <= k; i4++) {
    ss->data[i4] = b_ss->data[i4];
  }

  emxFree_real_T(&b_ss);

  /* 'builddetailedbf:10' sx = parevalspline(scoefx,si,ss,0); */
  parevalspline(scoefx, si, ss, sx);

  /* 'builddetailedbf:11' sy = parevalspline(scoefy,si,ss,0); */
  parevalspline(scoefy, si, ss, sy);
}

/* End of code generation (builddetailedbf.cpp) */
