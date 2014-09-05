/*
 * builddetailedbf.cpp
 *
 * Code generation for function 'builddetailedbf'
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
#include "matlab_emxutil.h"
#include "linspace.h"

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
  emxArray_real_T *b_ss;
  real_T distance;
  int32_T i6;
  int32_T loop_ub;
  b_emxInit_real_T(&b_ss, 2);

  /* 'builddetailedbf:3' distance = si(end) - si(1); */
  distance = si->data[si->size[0] - 1] - si->data[0];

  /* 'builddetailedbf:4' npoints = ceil(distance/gran); */
  /* round to next integer. Favoring increased granularity. */
  /* 'builddetailedbf:6' ss = linspace(si(1),si(end),npoints); */
  linspace(si->data[0], si->data[si->size[0] - 1], ceil(distance / gran), b_ss);

  /* 'builddetailedbf:8' ss = ss'; */
  i6 = ss->size[0];
  ss->size[0] = b_ss->size[1];
  emxEnsureCapacity((emxArray__common *)ss, i6, (int32_T)sizeof(real_T));
  loop_ub = b_ss->size[1] - 1;
  for (i6 = 0; i6 <= loop_ub; i6++) {
    ss->data[i6] = b_ss->data[i6];
  }

  emxFree_real_T(&b_ss);

  /* 'builddetailedbf:10' sx = parevalspline(scoefx,si,ss,0); */
  b_parevalspline(scoefx, si, ss, sx);

  /* 'builddetailedbf:11' sy = parevalspline(scoefy,si,ss,0); */
  b_parevalspline(scoefy, si, ss, sy);
}

/* End of code generation (builddetailedbf.cpp) */
