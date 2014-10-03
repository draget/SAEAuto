/*
 * parevalspline.cpp
 *
 * Code generation for function 'parevalspline'
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
#include "power.h"
#include "histc.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function [result, curvn] = parevalspline(coefs,breaks,t,d)
 */
void b_parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks,
                     const emxArray_real_T *t, emxArray_real_T *result)
{
  emxArray_real_T *hist;
  int32_T i9;
  int32_T loop_ub;
  emxArray_real_T *b_hist;
  emxArray_real_T *curvn;
  emxArray_real_T *b_t;
  emxArray_real_T *c_t;
  emxArray_real_T *r10;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i9 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i9, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i9 = 0; i9 <= loop_ub; i9++) {
    hist->data[i9] = breaks->data[i9];
  }

  emxInit_real_T(&b_hist, 1);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  i9 = b_hist->size[0];
  b_hist->size[0] = hist->size[0];
  emxEnsureCapacity((emxArray__common *)b_hist, i9, (int32_T)sizeof(real_T));
  loop_ub = hist->size[0] - 1;
  for (i9 = 0; i9 <= loop_ub; i9++) {
    b_hist->data[i9] = hist->data[i9];
  }

  emxInit_real_T(&curvn, 1);
  emxInit_real_T(&b_t, 1);
  histc(t, b_hist, hist, curvn);

  /* curvn = zeros(length(t),1); */
  /* for i = 1:length(t) */
  /*     j = 2; */
  /*     while t(i) > hist(j) */
  /*         j = j + 1 */
  /*         t(i) */
  /*         hist(j) */
  /*     end */
  /*     curvn(i) = j-1; */
  /*     if curvn(i) == length(hist) */
  /*         curvn(i) = curvn(i) - 1; */
  /*     end */
  /* end */
  /* 'parevalspline:22' if d == 0 */
  /* 'parevalspline:23' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:24'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
  i9 = b_t->size[0];
  b_t->size[0] = t->size[0];
  emxEnsureCapacity((emxArray__common *)b_t, i9, (int32_T)sizeof(real_T));
  emxFree_real_T(&b_hist);
  loop_ub = t->size[0] - 1;
  for (i9 = 0; i9 <= loop_ub; i9++) {
    b_t->data[i9] = t->data[i9] - breaks->data[(int32_T)curvn->data[i9] - 1];
  }

  emxInit_real_T(&c_t, 1);
  b_power(b_t, hist);
  i9 = c_t->size[0];
  c_t->size[0] = t->size[0];
  emxEnsureCapacity((emxArray__common *)c_t, i9, (int32_T)sizeof(real_T));
  emxFree_real_T(&b_t);
  loop_ub = t->size[0] - 1;
  for (i9 = 0; i9 <= loop_ub; i9++) {
    c_t->data[i9] = t->data[i9] - breaks->data[(int32_T)curvn->data[i9] - 1];
  }

  emxInit_real_T(&r10, 1);
  power(c_t, r10);
  i9 = result->size[0];
  result->size[0] = curvn->size[0];
  emxEnsureCapacity((emxArray__common *)result, i9, (int32_T)sizeof(real_T));
  emxFree_real_T(&c_t);
  loop_ub = curvn->size[0] - 1;
  for (i9 = 0; i9 <= loop_ub; i9++) {
    result->data[i9] = ((coefs->data[(int32_T)curvn->data[i9] - 1] * hist->
                         data[i9] + coefs->data[((int32_T)curvn->data[i9] +
      coefs->size[0]) - 1] * r10->data[i9]) + coefs->data[((int32_T)curvn->
      data[i9] + (coefs->size[0] << 1)) - 1] * (t->data[i9] - breaks->data
      [(int32_T)curvn->data[i9] - 1])) + coefs->data[((int32_T)curvn->data[i9] +
      coefs->size[0] * 3) - 1];
  }

  emxFree_real_T(&r10);
  emxFree_real_T(&curvn);
  emxFree_real_T(&hist);
}

/*
 * function [result, curvn] = parevalspline(coefs,breaks,t,d)
 */
void c_parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks,
                     real_T t, real_T *result, real_T *curvn)
{
  emxArray_real_T *hist;
  int32_T i19;
  int32_T loop_ub;
  emxArray_real_T *unusedU0;
  real_T a;
  real_T b_a;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i19 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i19, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i19 = 0; i19 <= loop_ub; i19++) {
    hist->data[i19] = breaks->data[i19];
  }

  b_emxInit_real_T(&unusedU0, 2);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  b_histc(t, hist, unusedU0, curvn);

  /* curvn = zeros(length(t),1); */
  /* for i = 1:length(t) */
  /*     j = 2; */
  /*     while t(i) > hist(j) */
  /*         j = j + 1 */
  /*         t(i) */
  /*         hist(j) */
  /*     end */
  /*     curvn(i) = j-1; */
  /*     if curvn(i) == length(hist) */
  /*         curvn(i) = curvn(i) - 1; */
  /*     end */
  /* end */
  /* 'parevalspline:22' if d == 0 */
  /* 'parevalspline:23' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:24'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
  a = t - breaks->data[(int32_T)*curvn - 1];
  b_a = t - breaks->data[(int32_T)*curvn - 1];
  *result = ((coefs->data[(int32_T)*curvn - 1] * rt_powd_snf(a, 3.0) +
              coefs->data[((int32_T)*curvn + coefs->size[0]) - 1] * rt_powd_snf
              (b_a, 2.0)) + coefs->data[((int32_T)*curvn + (coefs->size[0] << 1))
             - 1] * (t - breaks->data[(int32_T)*curvn - 1])) + coefs->data
    [((int32_T)*curvn + coefs->size[0] * 3) - 1];
  emxFree_real_T(&unusedU0);
  emxFree_real_T(&hist);
}

/*
 * function [result, curvn] = parevalspline(coefs,breaks,t,d)
 */
real_T d_parevalspline(const emxArray_real_T *coefs, const emxArray_real_T
  *breaks, real_T t)
{
  real_T result;
  emxArray_real_T *hist;
  int32_T i20;
  int32_T loop_ub;
  emxArray_real_T *unusedU0;
  real_T curvn;
  real_T a;
  real_T b_a;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i20 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i20, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i20 = 0; i20 <= loop_ub; i20++) {
    hist->data[i20] = breaks->data[i20];
  }

  b_emxInit_real_T(&unusedU0, 2);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  b_histc(t, hist, unusedU0, &curvn);

  /* curvn = zeros(length(t),1); */
  /* for i = 1:length(t) */
  /*     j = 2; */
  /*     while t(i) > hist(j) */
  /*         j = j + 1 */
  /*         t(i) */
  /*         hist(j) */
  /*     end */
  /*     curvn(i) = j-1; */
  /*     if curvn(i) == length(hist) */
  /*         curvn(i) = curvn(i) - 1; */
  /*     end */
  /* end */
  /* 'parevalspline:22' if d == 0 */
  /* 'parevalspline:23' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:24'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
  a = t - breaks->data[(int32_T)curvn - 1];
  b_a = t - breaks->data[(int32_T)curvn - 1];
  result = ((coefs->data[(int32_T)curvn - 1] * rt_powd_snf(a, 3.0) + coefs->
             data[((int32_T)curvn + coefs->size[0]) - 1] * rt_powd_snf(b_a, 2.0))
            + coefs->data[((int32_T)curvn + (coefs->size[0] << 1)) - 1] * (t -
             breaks->data[(int32_T)curvn - 1])) + coefs->data[((int32_T)curvn +
    coefs->size[0] * 3) - 1];
  emxFree_real_T(&unusedU0);
  emxFree_real_T(&hist);
  return result;
}

/*
 * function [result, curvn] = parevalspline(coefs,breaks,t,d)
 */
void e_parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks,
                     real_T t, real_T *result, real_T *curvn)
{
  emxArray_real_T *hist;
  int32_T i21;
  int32_T loop_ub;
  emxArray_real_T *unusedU0;
  real_T a;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i21 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i21, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i21 = 0; i21 <= loop_ub; i21++) {
    hist->data[i21] = breaks->data[i21];
  }

  b_emxInit_real_T(&unusedU0, 2);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  b_histc(t, hist, unusedU0, curvn);

  /* curvn = zeros(length(t),1); */
  /* for i = 1:length(t) */
  /*     j = 2; */
  /*     while t(i) > hist(j) */
  /*         j = j + 1 */
  /*         t(i) */
  /*         hist(j) */
  /*     end */
  /*     curvn(i) = j-1; */
  /*     if curvn(i) == length(hist) */
  /*         curvn(i) = curvn(i) - 1; */
  /*     end */
  /* end */
  /* 'parevalspline:22' if d == 0 */
  /* 'parevalspline:25' else */
  /* 'parevalspline:26' if d == 1 */
  /* 'parevalspline:27' result = coefs(curvn,3)+ 2.*coefs(curvn,2).*(t-breaks(curvn)) + 3.*coefs(curvn,1).*((t-breaks(curvn)).^2); */
  a = t - breaks->data[(int32_T)*curvn - 1];
  *result = (coefs->data[((int32_T)*curvn + (coefs->size[0] << 1)) - 1] + 2.0 *
             coefs->data[((int32_T)*curvn + coefs->size[0]) - 1] * (t -
              breaks->data[(int32_T)*curvn - 1])) + 3.0 * coefs->data[(int32_T)
    *curvn - 1] * rt_powd_snf(a, 2.0);
  emxFree_real_T(&unusedU0);
  emxFree_real_T(&hist);
}

/*
 * function [result, curvn] = parevalspline(coefs,breaks,t,d)
 */
void f_parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks,
                     real_T t, real_T *result, real_T *curvn)
{
  emxArray_real_T *hist;
  int32_T i22;
  int32_T loop_ub;
  emxArray_real_T *unusedU0;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i22 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i22, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i22 = 0; i22 <= loop_ub; i22++) {
    hist->data[i22] = breaks->data[i22];
  }

  b_emxInit_real_T(&unusedU0, 2);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  b_histc(t, hist, unusedU0, curvn);

  /* curvn = zeros(length(t),1); */
  /* for i = 1:length(t) */
  /*     j = 2; */
  /*     while t(i) > hist(j) */
  /*         j = j + 1 */
  /*         t(i) */
  /*         hist(j) */
  /*     end */
  /*     curvn(i) = j-1; */
  /*     if curvn(i) == length(hist) */
  /*         curvn(i) = curvn(i) - 1; */
  /*     end */
  /* end */
  /* 'parevalspline:22' if d == 0 */
  /* 'parevalspline:25' else */
  /* 'parevalspline:26' if d == 1 */
  /* 'parevalspline:28' else */
  /* 'parevalspline:28' if d == 2 */
  /* 'parevalspline:29' result = 2.*coefs(curvn,2) + 6.*coefs(curvn,1).*(t-breaks(curvn)); */
  *result = 2.0 * coefs->data[((int32_T)*curvn + coefs->size[0]) - 1] + 6.0 *
    coefs->data[(int32_T)*curvn - 1] * (t - breaks->data[(int32_T)*curvn - 1]);
  emxFree_real_T(&unusedU0);
  emxFree_real_T(&hist);
}

/*
 * function [result, curvn] = parevalspline(coefs,breaks,t,d)
 */
void parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks,
                   const emxArray_real_T *t, real_T d, emxArray_real_T *result,
                   emxArray_real_T *curvn)
{
  emxArray_real_T *hist;
  int32_T i23;
  int32_T loop_ub;
  emxArray_real_T *b_hist;
  emxArray_real_T *r28;
  emxArray_real_T *b_t;
  emxArray_real_T *c_t;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i23 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i23, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i23 = 0; i23 <= loop_ub; i23++) {
    hist->data[i23] = breaks->data[i23];
  }

  emxInit_real_T(&b_hist, 1);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  i23 = b_hist->size[0];
  b_hist->size[0] = hist->size[0];
  emxEnsureCapacity((emxArray__common *)b_hist, i23, (int32_T)sizeof(real_T));
  loop_ub = hist->size[0] - 1;
  for (i23 = 0; i23 <= loop_ub; i23++) {
    b_hist->data[i23] = hist->data[i23];
  }

  histc(t, b_hist, hist, curvn);

  /* curvn = zeros(length(t),1); */
  /* for i = 1:length(t) */
  /*     j = 2; */
  /*     while t(i) > hist(j) */
  /*         j = j + 1 */
  /*         t(i) */
  /*         hist(j) */
  /*     end */
  /*     curvn(i) = j-1; */
  /*     if curvn(i) == length(hist) */
  /*         curvn(i) = curvn(i) - 1; */
  /*     end */
  /* end */
  /* 'parevalspline:22' if d == 0 */
  emxFree_real_T(&b_hist);
  emxInit_real_T(&r28, 1);
  if (d == 0.0) {
    emxInit_real_T(&b_t, 1);

    /* 'parevalspline:23' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
    /* 'parevalspline:24'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
    i23 = b_t->size[0];
    b_t->size[0] = t->size[0];
    emxEnsureCapacity((emxArray__common *)b_t, i23, (int32_T)sizeof(real_T));
    loop_ub = t->size[0] - 1;
    for (i23 = 0; i23 <= loop_ub; i23++) {
      b_t->data[i23] = t->data[i23] - breaks->data[(int32_T)curvn->data[i23] - 1];
    }

    emxInit_real_T(&c_t, 1);
    b_power(b_t, hist);
    i23 = c_t->size[0];
    c_t->size[0] = t->size[0];
    emxEnsureCapacity((emxArray__common *)c_t, i23, (int32_T)sizeof(real_T));
    emxFree_real_T(&b_t);
    loop_ub = t->size[0] - 1;
    for (i23 = 0; i23 <= loop_ub; i23++) {
      c_t->data[i23] = t->data[i23] - breaks->data[(int32_T)curvn->data[i23] - 1];
    }

    power(c_t, r28);
    i23 = result->size[0];
    result->size[0] = curvn->size[0];
    emxEnsureCapacity((emxArray__common *)result, i23, (int32_T)sizeof(real_T));
    emxFree_real_T(&c_t);
    loop_ub = curvn->size[0] - 1;
    for (i23 = 0; i23 <= loop_ub; i23++) {
      result->data[i23] = ((coefs->data[(int32_T)curvn->data[i23] - 1] *
                            hist->data[i23] + coefs->data[((int32_T)curvn->
        data[i23] + coefs->size[0]) - 1] * r28->data[i23]) + coefs->data
                           [((int32_T)curvn->data[i23] + (coefs->size[0] << 1))
                           - 1] * (t->data[i23] - breaks->data[(int32_T)
        curvn->data[i23] - 1])) + coefs->data[((int32_T)curvn->data[i23] +
        coefs->size[0] * 3) - 1];
    }
  } else {
    /* 'parevalspline:25' else */
    /* 'parevalspline:26' if d == 1 */
    if (d == 1.0) {
      /* 'parevalspline:27' result = coefs(curvn,3)+ 2.*coefs(curvn,2).*(t-breaks(curvn)) + 3.*coefs(curvn,1).*((t-breaks(curvn)).^2); */
      i23 = hist->size[0];
      hist->size[0] = curvn->size[0];
      emxEnsureCapacity((emxArray__common *)hist, i23, (int32_T)sizeof(real_T));
      loop_ub = curvn->size[0] - 1;
      for (i23 = 0; i23 <= loop_ub; i23++) {
        hist->data[i23] = 2.0 * coefs->data[((int32_T)curvn->data[i23] +
          coefs->size[0]) - 1];
      }

      i23 = r28->size[0];
      r28->size[0] = curvn->size[0];
      emxEnsureCapacity((emxArray__common *)r28, i23, (int32_T)sizeof(real_T));
      loop_ub = curvn->size[0] - 1;
      for (i23 = 0; i23 <= loop_ub; i23++) {
        r28->data[i23] = 3.0 * coefs->data[(int32_T)curvn->data[i23] - 1];
      }

      emxInit_real_T(&b_t, 1);
      i23 = b_t->size[0];
      b_t->size[0] = t->size[0];
      emxEnsureCapacity((emxArray__common *)b_t, i23, (int32_T)sizeof(real_T));
      loop_ub = t->size[0] - 1;
      for (i23 = 0; i23 <= loop_ub; i23++) {
        b_t->data[i23] = t->data[i23] - breaks->data[(int32_T)curvn->data[i23] -
          1];
      }

      emxInit_real_T(&c_t, 1);
      power(b_t, c_t);
      i23 = result->size[0];
      result->size[0] = curvn->size[0];
      emxEnsureCapacity((emxArray__common *)result, i23, (int32_T)sizeof(real_T));
      emxFree_real_T(&b_t);
      loop_ub = curvn->size[0] - 1;
      for (i23 = 0; i23 <= loop_ub; i23++) {
        result->data[i23] = (coefs->data[((int32_T)curvn->data[i23] +
          (coefs->size[0] << 1)) - 1] + hist->data[i23] * (t->data[i23] -
          breaks->data[(int32_T)curvn->data[i23] - 1])) + r28->data[i23] *
          c_t->data[i23];
      }

      emxFree_real_T(&c_t);
    } else {
      /* 'parevalspline:28' else */
      /* 'parevalspline:28' if d == 2 */
      if (d == 2.0) {
        /* 'parevalspline:29' result = 2.*coefs(curvn,2) + 6.*coefs(curvn,1).*(t-breaks(curvn)); */
        i23 = hist->size[0];
        hist->size[0] = curvn->size[0];
        emxEnsureCapacity((emxArray__common *)hist, i23, (int32_T)sizeof(real_T));
        loop_ub = curvn->size[0] - 1;
        for (i23 = 0; i23 <= loop_ub; i23++) {
          hist->data[i23] = 6.0 * coefs->data[(int32_T)curvn->data[i23] - 1];
        }

        i23 = r28->size[0];
        r28->size[0] = curvn->size[0];
        emxEnsureCapacity((emxArray__common *)r28, i23, (int32_T)sizeof(real_T));
        loop_ub = curvn->size[0] - 1;
        for (i23 = 0; i23 <= loop_ub; i23++) {
          r28->data[i23] = 2.0 * coefs->data[((int32_T)curvn->data[i23] +
            coefs->size[0]) - 1];
        }

        i23 = result->size[0];
        result->size[0] = r28->size[0];
        emxEnsureCapacity((emxArray__common *)result, i23, (int32_T)sizeof
                          (real_T));
        loop_ub = r28->size[0] - 1;
        for (i23 = 0; i23 <= loop_ub; i23++) {
          result->data[i23] = r28->data[i23] + hist->data[i23] * (t->data[i23] -
            breaks->data[(int32_T)curvn->data[i23] - 1]);
        }
      } else {
        /* 'parevalspline:30' else */
        /* 'parevalspline:31' result = 0; */
        i23 = result->size[0];
        result->size[0] = 1;
        emxEnsureCapacity((emxArray__common *)result, i23, (int32_T)sizeof
                          (real_T));
        result->data[0] = 0.0;

        /* Incorrect requested d */
      }
    }
  }

  emxFree_real_T(&r28);
  emxFree_real_T(&hist);
}

/* End of code generation (parevalspline.cpp) */
