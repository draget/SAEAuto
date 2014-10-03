/*
 * dissqrd.cpp
 *
 * Code generation for function 'dissqrd'
 *
 * C source code generated on: Thu Sep 18 17:26:04 2014
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
#include "dissqrd.h"
#include "matlab_emxutil.h"
#include "histc.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function [distance, curvn] = dissqrd(coefx,coefy,breaks,s,x0,y0)
 */
void dissqrd(const emxArray_real_T *coefx, const emxArray_real_T *coefy, const
             emxArray_real_T *breaks, const real_T s[4], real_T x0, real_T b_y0,
             real_T distance[4])
{
  emxArray_real_T *hist;
  int32_T k;
  int32_T loop_ub;
  emxArray_real_T *b_hist;
  real_T curvn[4];
  real_T ys[4];
  real_T y[4];
  real_T b_y[4];
  real_T xs[4];
  emxArray_real_T *c_hist;
  emxInit_real_T(&hist, 1);

  /* UNTITLED2 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0) */
  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  k = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, k, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (k = 0; k <= loop_ub; k++) {
    hist->data[k] = breaks->data[k];
  }

  emxInit_real_T(&b_hist, 1);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  k = b_hist->size[0];
  b_hist->size[0] = hist->size[0];
  emxEnsureCapacity((emxArray__common *)b_hist, k, (int32_T)sizeof(real_T));
  loop_ub = hist->size[0] - 1;
  for (k = 0; k <= loop_ub; k++) {
    b_hist->data[k] = hist->data[k];
  }

  c_histc(s, b_hist, hist, curvn);

  /* 'parevalspline:9' if d == 0 */
  /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
  emxFree_real_T(&b_hist);
  for (k = 0; k < 4; k++) {
    ys[k] = s[k] - breaks->data[(int32_T)curvn[k] - 1];
  }

  for (k = 0; k < 4; k++) {
    y[k] = rt_powd_snf(ys[k], 3.0);
  }

  for (k = 0; k < 4; k++) {
    ys[k] = s[k] - breaks->data[(int32_T)curvn[k] - 1];
  }

  for (k = 0; k < 4; k++) {
    b_y[k] = rt_powd_snf(ys[k], 2.0);
  }

  for (k = 0; k < 4; k++) {
    xs[k] = ((coefx->data[(int32_T)curvn[k] - 1] * y[k] + coefx->data[((int32_T)
               curvn[k] + coefx->size[0]) - 1] * b_y[k]) + coefx->data[((int32_T)
              curvn[k] + (coefx->size[0] << 1)) - 1] * (s[k] - breaks->data
              [(int32_T)curvn[k] - 1])) + coefx->data[((int32_T)curvn[k] +
      coefx->size[0] * 3) - 1];
  }

  /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0) */
  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  k = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, k, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (k = 0; k <= loop_ub; k++) {
    hist->data[k] = breaks->data[k];
  }

  emxInit_real_T(&c_hist, 1);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  k = c_hist->size[0];
  c_hist->size[0] = hist->size[0];
  emxEnsureCapacity((emxArray__common *)c_hist, k, (int32_T)sizeof(real_T));
  loop_ub = hist->size[0] - 1;
  for (k = 0; k <= loop_ub; k++) {
    c_hist->data[k] = hist->data[k];
  }

  c_histc(s, c_hist, hist, curvn);

  /* 'parevalspline:9' if d == 0 */
  /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
  emxFree_real_T(&c_hist);
  emxFree_real_T(&hist);
  for (k = 0; k < 4; k++) {
    ys[k] = s[k] - breaks->data[(int32_T)curvn[k] - 1];
  }

  for (k = 0; k < 4; k++) {
    y[k] = rt_powd_snf(ys[k], 3.0);
  }

  for (k = 0; k < 4; k++) {
    ys[k] = s[k] - breaks->data[(int32_T)curvn[k] - 1];
  }

  for (k = 0; k < 4; k++) {
    b_y[k] = rt_powd_snf(ys[k], 2.0);
  }

  for (k = 0; k < 4; k++) {
    ys[k] = ((coefy->data[(int32_T)curvn[k] - 1] * y[k] + coefy->data[((int32_T)
               curvn[k] + coefy->size[0]) - 1] * b_y[k]) + coefy->data[((int32_T)
              curvn[k] + (coefy->size[0] << 1)) - 1] * (s[k] - breaks->data
              [(int32_T)curvn[k] - 1])) + coefy->data[((int32_T)curvn[k] +
      coefy->size[0] * 3) - 1];
  }

  /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2 */
  for (k = 0; k < 4; k++) {
    distance[k] = rt_powd_snf(xs[k] - x0, 2.0) + rt_powd_snf(ys[k] - b_y0, 2.0);
  }
}

/* End of code generation (dissqrd.cpp) */
