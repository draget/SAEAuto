/*
 * parevalspline.cpp
 *
 * Code generation for function 'parevalspline'
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
  int32_T i8;
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
  i8 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i8, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i8 = 0; i8 <= loop_ub; i8++) {
    hist->data[i8] = breaks->data[i8];
  }

  emxInit_real_T(&b_hist, 1);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  i8 = b_hist->size[0];
  b_hist->size[0] = hist->size[0];
  emxEnsureCapacity((emxArray__common *)b_hist, i8, (int32_T)sizeof(real_T));
  loop_ub = hist->size[0] - 1;
  for (i8 = 0; i8 <= loop_ub; i8++) {
    b_hist->data[i8] = hist->data[i8];
  }

  emxInit_real_T(&curvn, 1);
  emxInit_real_T(&b_t, 1);
  histc(t, b_hist, hist, curvn);

  /* 'parevalspline:9' if d == 0 */
  /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
  i8 = b_t->size[0];
  b_t->size[0] = t->size[0];
  emxEnsureCapacity((emxArray__common *)b_t, i8, (int32_T)sizeof(real_T));
  emxFree_real_T(&b_hist);
  loop_ub = t->size[0] - 1;
  for (i8 = 0; i8 <= loop_ub; i8++) {
    b_t->data[i8] = t->data[i8] - breaks->data[(int32_T)curvn->data[i8] - 1];
  }

  emxInit_real_T(&c_t, 1);
  b_power(b_t, hist);
  i8 = c_t->size[0];
  c_t->size[0] = t->size[0];
  emxEnsureCapacity((emxArray__common *)c_t, i8, (int32_T)sizeof(real_T));
  emxFree_real_T(&b_t);
  loop_ub = t->size[0] - 1;
  for (i8 = 0; i8 <= loop_ub; i8++) {
    c_t->data[i8] = t->data[i8] - breaks->data[(int32_T)curvn->data[i8] - 1];
  }

  emxInit_real_T(&r10, 1);
  power(c_t, r10);
  i8 = result->size[0];
  result->size[0] = curvn->size[0];
  emxEnsureCapacity((emxArray__common *)result, i8, (int32_T)sizeof(real_T));
  emxFree_real_T(&c_t);
  loop_ub = curvn->size[0] - 1;
  for (i8 = 0; i8 <= loop_ub; i8++) {
    result->data[i8] = ((coefs->data[(int32_T)curvn->data[i8] - 1] * hist->
                         data[i8] + coefs->data[((int32_T)curvn->data[i8] +
      coefs->size[0]) - 1] * r10->data[i8]) + coefs->data[((int32_T)curvn->
      data[i8] + (coefs->size[0] << 1)) - 1] * (t->data[i8] - breaks->data
      [(int32_T)curvn->data[i8] - 1])) + coefs->data[((int32_T)curvn->data[i8] +
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
  int32_T i14;
  int32_T loop_ub;
  emxArray_real_T *unusedU0;
  real_T a;
  real_T b_a;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i14 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i14, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    hist->data[i14] = breaks->data[i14];
  }

  b_emxInit_real_T(&unusedU0, 2);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  b_histc(t, hist, unusedU0, curvn);

  /* 'parevalspline:9' if d == 0 */
  /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
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
  int32_T i15;
  int32_T loop_ub;
  emxArray_real_T *unusedU0;
  real_T curvn;
  real_T a;
  real_T b_a;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i15 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i15, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i15 = 0; i15 <= loop_ub; i15++) {
    hist->data[i15] = breaks->data[i15];
  }

  b_emxInit_real_T(&unusedU0, 2);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  b_histc(t, hist, unusedU0, &curvn);

  /* 'parevalspline:9' if d == 0 */
  /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
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
void parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks,
                   const emxArray_real_T *t, real_T d, emxArray_real_T *result,
                   emxArray_real_T *curvn)
{
  emxArray_real_T *hist;
  int32_T i16;
  int32_T loop_ub;
  emxArray_real_T *b_hist;
  emxArray_real_T *r76;
  emxArray_real_T *b_t;
  emxArray_real_T *c_t;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i16 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i16, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i16 = 0; i16 <= loop_ub; i16++) {
    hist->data[i16] = breaks->data[i16];
  }

  emxInit_real_T(&b_hist, 1);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  i16 = b_hist->size[0];
  b_hist->size[0] = hist->size[0];
  emxEnsureCapacity((emxArray__common *)b_hist, i16, (int32_T)sizeof(real_T));
  loop_ub = hist->size[0] - 1;
  for (i16 = 0; i16 <= loop_ub; i16++) {
    b_hist->data[i16] = hist->data[i16];
  }

  histc(t, b_hist, hist, curvn);

  /* 'parevalspline:9' if d == 0 */
  emxFree_real_T(&b_hist);
  emxInit_real_T(&r76, 1);
  if (d == 0.0) {
    emxInit_real_T(&b_t, 1);

    /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
    /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
    i16 = b_t->size[0];
    b_t->size[0] = t->size[0];
    emxEnsureCapacity((emxArray__common *)b_t, i16, (int32_T)sizeof(real_T));
    loop_ub = t->size[0] - 1;
    for (i16 = 0; i16 <= loop_ub; i16++) {
      b_t->data[i16] = t->data[i16] - breaks->data[(int32_T)curvn->data[i16] - 1];
    }

    emxInit_real_T(&c_t, 1);
    b_power(b_t, hist);
    i16 = c_t->size[0];
    c_t->size[0] = t->size[0];
    emxEnsureCapacity((emxArray__common *)c_t, i16, (int32_T)sizeof(real_T));
    emxFree_real_T(&b_t);
    loop_ub = t->size[0] - 1;
    for (i16 = 0; i16 <= loop_ub; i16++) {
      c_t->data[i16] = t->data[i16] - breaks->data[(int32_T)curvn->data[i16] - 1];
    }

    power(c_t, r76);
    i16 = result->size[0];
    result->size[0] = curvn->size[0];
    emxEnsureCapacity((emxArray__common *)result, i16, (int32_T)sizeof(real_T));
    emxFree_real_T(&c_t);
    loop_ub = curvn->size[0] - 1;
    for (i16 = 0; i16 <= loop_ub; i16++) {
      result->data[i16] = ((coefs->data[(int32_T)curvn->data[i16] - 1] *
                            hist->data[i16] + coefs->data[((int32_T)curvn->
        data[i16] + coefs->size[0]) - 1] * r76->data[i16]) + coefs->data
                           [((int32_T)curvn->data[i16] + (coefs->size[0] << 1))
                           - 1] * (t->data[i16] - breaks->data[(int32_T)
        curvn->data[i16] - 1])) + coefs->data[((int32_T)curvn->data[i16] +
        coefs->size[0] * 3) - 1];
    }
  } else {
    /* 'parevalspline:12' else */
    /* 'parevalspline:13' if d == 1 */
    if (d == 1.0) {
      /* 'parevalspline:14' result = coefs(curvn,3)+ 2.*coefs(curvn,2).*(t-breaks(curvn)) + 3.*coefs(curvn,1).*((t-breaks(curvn)).^2); */
      i16 = hist->size[0];
      hist->size[0] = curvn->size[0];
      emxEnsureCapacity((emxArray__common *)hist, i16, (int32_T)sizeof(real_T));
      loop_ub = curvn->size[0] - 1;
      for (i16 = 0; i16 <= loop_ub; i16++) {
        hist->data[i16] = 2.0 * coefs->data[((int32_T)curvn->data[i16] +
          coefs->size[0]) - 1];
      }

      i16 = r76->size[0];
      r76->size[0] = curvn->size[0];
      emxEnsureCapacity((emxArray__common *)r76, i16, (int32_T)sizeof(real_T));
      loop_ub = curvn->size[0] - 1;
      for (i16 = 0; i16 <= loop_ub; i16++) {
        r76->data[i16] = 3.0 * coefs->data[(int32_T)curvn->data[i16] - 1];
      }

      emxInit_real_T(&b_t, 1);
      i16 = b_t->size[0];
      b_t->size[0] = t->size[0];
      emxEnsureCapacity((emxArray__common *)b_t, i16, (int32_T)sizeof(real_T));
      loop_ub = t->size[0] - 1;
      for (i16 = 0; i16 <= loop_ub; i16++) {
        b_t->data[i16] = t->data[i16] - breaks->data[(int32_T)curvn->data[i16] -
          1];
      }

      emxInit_real_T(&c_t, 1);
      power(b_t, c_t);
      i16 = result->size[0];
      result->size[0] = curvn->size[0];
      emxEnsureCapacity((emxArray__common *)result, i16, (int32_T)sizeof(real_T));
      emxFree_real_T(&b_t);
      loop_ub = curvn->size[0] - 1;
      for (i16 = 0; i16 <= loop_ub; i16++) {
        result->data[i16] = (coefs->data[((int32_T)curvn->data[i16] +
          (coefs->size[0] << 1)) - 1] + hist->data[i16] * (t->data[i16] -
          breaks->data[(int32_T)curvn->data[i16] - 1])) + r76->data[i16] *
          c_t->data[i16];
      }

      emxFree_real_T(&c_t);
    } else {
      /* 'parevalspline:15' else */
      /* 'parevalspline:15' if d == 2 */
      if (d == 2.0) {
        /* 'parevalspline:16' result = 2.*coefs(curvn,2) + 6.*coefs(curvn,1).*(t-breaks(curvn)); */
        i16 = hist->size[0];
        hist->size[0] = curvn->size[0];
        emxEnsureCapacity((emxArray__common *)hist, i16, (int32_T)sizeof(real_T));
        loop_ub = curvn->size[0] - 1;
        for (i16 = 0; i16 <= loop_ub; i16++) {
          hist->data[i16] = 6.0 * coefs->data[(int32_T)curvn->data[i16] - 1];
        }

        i16 = r76->size[0];
        r76->size[0] = curvn->size[0];
        emxEnsureCapacity((emxArray__common *)r76, i16, (int32_T)sizeof(real_T));
        loop_ub = curvn->size[0] - 1;
        for (i16 = 0; i16 <= loop_ub; i16++) {
          r76->data[i16] = 2.0 * coefs->data[((int32_T)curvn->data[i16] +
            coefs->size[0]) - 1];
        }

        i16 = result->size[0];
        result->size[0] = r76->size[0];
        emxEnsureCapacity((emxArray__common *)result, i16, (int32_T)sizeof
                          (real_T));
        loop_ub = r76->size[0] - 1;
        for (i16 = 0; i16 <= loop_ub; i16++) {
          result->data[i16] = r76->data[i16] + hist->data[i16] * (t->data[i16] -
            breaks->data[(int32_T)curvn->data[i16] - 1]);
        }
      } else {
        /* 'parevalspline:17' else */
        /* 'parevalspline:18' result = 0; */
        i16 = result->size[0];
        result->size[0] = 1;
        emxEnsureCapacity((emxArray__common *)result, i16, (int32_T)sizeof
                          (real_T));
        result->data[0] = 0.0;

        /* Incorrect requested d */
      }
    }
  }

  emxFree_real_T(&r76);
  emxFree_real_T(&hist);
}

/* End of code generation (parevalspline.cpp) */
