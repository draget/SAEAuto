/*
 * parevalspline.cpp
 *
 * Code generation for function 'parevalspline'
 *
 * C source code generated on: Thu Aug  7 12:12:52 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "buildbfcurvature.h"
#include "builddetailedbf.h"
#include "localize.h"
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
  int32_T i7;
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
  i7 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i7, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i7 = 0; i7 <= loop_ub; i7++) {
    hist->data[i7] = breaks->data[i7];
  }

  emxInit_real_T(&b_hist, 1);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  i7 = b_hist->size[0];
  b_hist->size[0] = hist->size[0];
  emxEnsureCapacity((emxArray__common *)b_hist, i7, (int32_T)sizeof(real_T));
  loop_ub = hist->size[0] - 1;
  for (i7 = 0; i7 <= loop_ub; i7++) {
    b_hist->data[i7] = hist->data[i7];
  }

  emxInit_real_T(&curvn, 1);
  emxInit_real_T(&b_t, 1);
  histc(t, b_hist, hist, curvn);

  /* 'parevalspline:9' if d == 0 */
  /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
  i7 = b_t->size[0];
  b_t->size[0] = t->size[0];
  emxEnsureCapacity((emxArray__common *)b_t, i7, (int32_T)sizeof(real_T));
  emxFree_real_T(&b_hist);
  loop_ub = t->size[0] - 1;
  for (i7 = 0; i7 <= loop_ub; i7++) {
    b_t->data[i7] = t->data[i7] - breaks->data[(int32_T)curvn->data[i7] - 1];
  }

  emxInit_real_T(&c_t, 1);
  b_power(b_t, hist);
  i7 = c_t->size[0];
  c_t->size[0] = t->size[0];
  emxEnsureCapacity((emxArray__common *)c_t, i7, (int32_T)sizeof(real_T));
  emxFree_real_T(&b_t);
  loop_ub = t->size[0] - 1;
  for (i7 = 0; i7 <= loop_ub; i7++) {
    c_t->data[i7] = t->data[i7] - breaks->data[(int32_T)curvn->data[i7] - 1];
  }

  emxInit_real_T(&r10, 1);
  power(c_t, r10);
  i7 = result->size[0];
  result->size[0] = curvn->size[0];
  emxEnsureCapacity((emxArray__common *)result, i7, (int32_T)sizeof(real_T));
  emxFree_real_T(&c_t);
  loop_ub = curvn->size[0] - 1;
  for (i7 = 0; i7 <= loop_ub; i7++) {
    result->data[i7] = ((coefs->data[(int32_T)curvn->data[i7] - 1] * hist->
                         data[i7] + coefs->data[((int32_T)curvn->data[i7] +
      coefs->size[0]) - 1] * r10->data[i7]) + coefs->data[((int32_T)curvn->
      data[i7] + (coefs->size[0] << 1)) - 1] * (t->data[i7] - breaks->data
      [(int32_T)curvn->data[i7] - 1])) + coefs->data[((int32_T)curvn->data[i7] +
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
  int32_T i11;
  int32_T loop_ub;
  emxArray_real_T *unusedU0;
  real_T a;
  real_T b_a;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i11 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i11, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i11 = 0; i11 <= loop_ub; i11++) {
    hist->data[i11] = breaks->data[i11];
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
  int32_T i12;
  int32_T loop_ub;
  emxArray_real_T *unusedU0;
  real_T curvn;
  real_T a;
  real_T b_a;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i12 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i12, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i12 = 0; i12 <= loop_ub; i12++) {
    hist->data[i12] = breaks->data[i12];
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
  int32_T i13;
  int32_T loop_ub;
  emxArray_real_T *b_hist;
  emxArray_real_T *r11;
  emxArray_real_T *b_t;
  emxArray_real_T *c_t;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  i13 = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, i13, (int32_T)sizeof(real_T));
  loop_ub = breaks->size[0] - 1;
  for (i13 = 0; i13 <= loop_ub; i13++) {
    hist->data[i13] = breaks->data[i13];
  }

  emxInit_real_T(&b_hist, 1);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  i13 = b_hist->size[0];
  b_hist->size[0] = hist->size[0];
  emxEnsureCapacity((emxArray__common *)b_hist, i13, (int32_T)sizeof(real_T));
  loop_ub = hist->size[0] - 1;
  for (i13 = 0; i13 <= loop_ub; i13++) {
    b_hist->data[i13] = hist->data[i13];
  }

  histc(t, b_hist, hist, curvn);

  /* 'parevalspline:9' if d == 0 */
  emxFree_real_T(&b_hist);
  emxInit_real_T(&r11, 1);
  if (d == 0.0) {
    emxInit_real_T(&b_t, 1);

    /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
    /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
    i13 = b_t->size[0];
    b_t->size[0] = t->size[0];
    emxEnsureCapacity((emxArray__common *)b_t, i13, (int32_T)sizeof(real_T));
    loop_ub = t->size[0] - 1;
    for (i13 = 0; i13 <= loop_ub; i13++) {
      b_t->data[i13] = t->data[i13] - breaks->data[(int32_T)curvn->data[i13] - 1];
    }

    emxInit_real_T(&c_t, 1);
    b_power(b_t, hist);
    i13 = c_t->size[0];
    c_t->size[0] = t->size[0];
    emxEnsureCapacity((emxArray__common *)c_t, i13, (int32_T)sizeof(real_T));
    emxFree_real_T(&b_t);
    loop_ub = t->size[0] - 1;
    for (i13 = 0; i13 <= loop_ub; i13++) {
      c_t->data[i13] = t->data[i13] - breaks->data[(int32_T)curvn->data[i13] - 1];
    }

    power(c_t, r11);
    i13 = result->size[0];
    result->size[0] = curvn->size[0];
    emxEnsureCapacity((emxArray__common *)result, i13, (int32_T)sizeof(real_T));
    emxFree_real_T(&c_t);
    loop_ub = curvn->size[0] - 1;
    for (i13 = 0; i13 <= loop_ub; i13++) {
      result->data[i13] = ((coefs->data[(int32_T)curvn->data[i13] - 1] *
                            hist->data[i13] + coefs->data[((int32_T)curvn->
        data[i13] + coefs->size[0]) - 1] * r11->data[i13]) + coefs->data
                           [((int32_T)curvn->data[i13] + (coefs->size[0] << 1))
                           - 1] * (t->data[i13] - breaks->data[(int32_T)
        curvn->data[i13] - 1])) + coefs->data[((int32_T)curvn->data[i13] +
        coefs->size[0] * 3) - 1];
    }
  } else {
    /* 'parevalspline:12' else */
    /* 'parevalspline:13' if d == 1 */
    if (d == 1.0) {
      /* 'parevalspline:14' result = coefs(curvn,3)+ 2.*coefs(curvn,2).*(t-breaks(curvn)) + 3.*coefs(curvn,1).*((t-breaks(curvn)).^2); */
      i13 = hist->size[0];
      hist->size[0] = curvn->size[0];
      emxEnsureCapacity((emxArray__common *)hist, i13, (int32_T)sizeof(real_T));
      loop_ub = curvn->size[0] - 1;
      for (i13 = 0; i13 <= loop_ub; i13++) {
        hist->data[i13] = 2.0 * coefs->data[((int32_T)curvn->data[i13] +
          coefs->size[0]) - 1];
      }

      i13 = r11->size[0];
      r11->size[0] = curvn->size[0];
      emxEnsureCapacity((emxArray__common *)r11, i13, (int32_T)sizeof(real_T));
      loop_ub = curvn->size[0] - 1;
      for (i13 = 0; i13 <= loop_ub; i13++) {
        r11->data[i13] = 3.0 * coefs->data[(int32_T)curvn->data[i13] - 1];
      }

      emxInit_real_T(&b_t, 1);
      i13 = b_t->size[0];
      b_t->size[0] = t->size[0];
      emxEnsureCapacity((emxArray__common *)b_t, i13, (int32_T)sizeof(real_T));
      loop_ub = t->size[0] - 1;
      for (i13 = 0; i13 <= loop_ub; i13++) {
        b_t->data[i13] = t->data[i13] - breaks->data[(int32_T)curvn->data[i13] -
          1];
      }

      emxInit_real_T(&c_t, 1);
      power(b_t, c_t);
      i13 = result->size[0];
      result->size[0] = curvn->size[0];
      emxEnsureCapacity((emxArray__common *)result, i13, (int32_T)sizeof(real_T));
      emxFree_real_T(&b_t);
      loop_ub = curvn->size[0] - 1;
      for (i13 = 0; i13 <= loop_ub; i13++) {
        result->data[i13] = (coefs->data[((int32_T)curvn->data[i13] +
          (coefs->size[0] << 1)) - 1] + hist->data[i13] * (t->data[i13] -
          breaks->data[(int32_T)curvn->data[i13] - 1])) + r11->data[i13] *
          c_t->data[i13];
      }

      emxFree_real_T(&c_t);
    } else {
      /* 'parevalspline:15' else */
      /* 'parevalspline:15' if d == 2 */
      if (d == 2.0) {
        /* 'parevalspline:16' result = 2.*coefs(curvn,2) + 6.*coefs(curvn,1).*(t-breaks(curvn)); */
        i13 = hist->size[0];
        hist->size[0] = curvn->size[0];
        emxEnsureCapacity((emxArray__common *)hist, i13, (int32_T)sizeof(real_T));
        loop_ub = curvn->size[0] - 1;
        for (i13 = 0; i13 <= loop_ub; i13++) {
          hist->data[i13] = 6.0 * coefs->data[(int32_T)curvn->data[i13] - 1];
        }

        i13 = r11->size[0];
        r11->size[0] = curvn->size[0];
        emxEnsureCapacity((emxArray__common *)r11, i13, (int32_T)sizeof(real_T));
        loop_ub = curvn->size[0] - 1;
        for (i13 = 0; i13 <= loop_ub; i13++) {
          r11->data[i13] = 2.0 * coefs->data[((int32_T)curvn->data[i13] +
            coefs->size[0]) - 1];
        }

        i13 = result->size[0];
        result->size[0] = r11->size[0];
        emxEnsureCapacity((emxArray__common *)result, i13, (int32_T)sizeof
                          (real_T));
        loop_ub = r11->size[0] - 1;
        for (i13 = 0; i13 <= loop_ub; i13++) {
          result->data[i13] = r11->data[i13] + hist->data[i13] * (t->data[i13] -
            breaks->data[(int32_T)curvn->data[i13] - 1]);
        }
      } else {
        /* 'parevalspline:17' else */
        /* 'parevalspline:18' result = 0; */
        i13 = result->size[0];
        result->size[0] = 1;
        emxEnsureCapacity((emxArray__common *)result, i13, (int32_T)sizeof
                          (real_T));
        result->data[0] = 0.0;

        /* Incorrect requested d */
      }
    }
  }

  emxFree_real_T(&r11);
  emxFree_real_T(&hist);
}

/* End of code generation (parevalspline.cpp) */
