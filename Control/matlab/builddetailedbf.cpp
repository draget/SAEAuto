/*
 * builddetailedbf.cpp
 *
 * Code generation for function 'builddetailedbf'
 *
 * C source code generated on: Wed Jul 23 14:27:34 2014
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
  int32_T n;
  real_T anew;
  real_T apnd;
  real_T ndbl;
  real_T cdiff;
  real_T absa;
  real_T absb;
  emxArray_real_T *y;
  int32_T nm1d2;
  int32_T nm1;
  int32_T k;

  /* 'builddetailedbf:3' ss = (si(1):gran:si(end))'; */
  if (rtIsNaN(si->data[0]) || rtIsNaN(gran) || rtIsNaN(si->data[si->size[0] - 1]))
  {
    n = 1;
    anew = rtNaN;
    apnd = si->data[si->size[0] - 1];
  } else if ((gran == 0.0) || ((si->data[0] < si->data[si->size[0] - 1]) &&
              (gran < 0.0)) || ((si->data[si->size[0] - 1] < si->data[0]) &&
              (gran > 0.0))) {
    n = 0;
    anew = si->data[0];
    apnd = si->data[si->size[0] - 1];
  } else if (rtIsInf(si->data[0]) || rtIsInf(gran) || rtIsInf(si->data[si->size
              [0] - 1])) {
    n = 1;
    anew = rtNaN;
    apnd = si->data[si->size[0] - 1];
  } else {
    anew = si->data[0];
    ndbl = floor((si->data[si->size[0] - 1] - si->data[0]) / gran + 0.5);
    apnd = si->data[0] + ndbl * gran;
    if (gran > 0.0) {
      cdiff = apnd - si->data[si->size[0] - 1];
    } else {
      cdiff = si->data[si->size[0] - 1] - apnd;
    }

    absa = fabs(si->data[0]);
    absb = fabs(si->data[si->size[0] - 1]);
    if (absa > absb) {
      absb = absa;
    }

    if (fabs(cdiff) < 4.4408920985006262E-16 * absb) {
      ndbl++;
      apnd = si->data[si->size[0] - 1];
    } else if (cdiff > 0.0) {
      apnd = si->data[0] + (ndbl - 1.0) * gran;
    } else {
      ndbl++;
    }

    if (ndbl >= 0.0) {
      n = (int32_T)ndbl;
    } else {
      n = 0;
    }
  }

  b_emxInit_real_T(&y, 2);
  nm1d2 = y->size[0] * y->size[1];
  y->size[0] = 1;
  y->size[1] = n;
  emxEnsureCapacity((emxArray__common *)y, nm1d2, (int32_T)sizeof(real_T));
  if (n > 0) {
    y->data[0] = anew;
    if (n > 1) {
      y->data[n - 1] = apnd;
      nm1 = n - 1;
      nm1d2 = nm1 / 2;
      for (k = 1; k <= nm1d2 - 1; k++) {
        ndbl = (real_T)k * gran;
        y->data[k] = anew + ndbl;
        y->data[(n - k) - 1] = apnd - ndbl;
      }

      if (nm1d2 << 1 == nm1) {
        y->data[nm1d2] = (anew + apnd) / 2.0;
      } else {
        ndbl = (real_T)nm1d2 * gran;
        y->data[nm1d2] = anew + ndbl;
        y->data[nm1d2 + 1] = apnd - ndbl;
      }
    }
  }

  nm1d2 = ss->size[0];
  ss->size[0] = y->size[1];
  emxEnsureCapacity((emxArray__common *)ss, nm1d2, (int32_T)sizeof(real_T));
  nm1 = y->size[1] - 1;
  for (nm1d2 = 0; nm1d2 <= nm1; nm1d2++) {
    ss->data[nm1d2] = y->data[nm1d2];
  }

  emxFree_real_T(&y);

  /* 'builddetailedbf:5' sx = parevalspline(scoefx,si,ss,0); */
  parevalspline(scoefx, si, ss, sx);

  /* 'builddetailedbf:6' sy = parevalspline(scoefy,si,ss,0); */
  parevalspline(scoefy, si, ss, sy);
}

/* End of code generation (builddetailedbf.cpp) */
