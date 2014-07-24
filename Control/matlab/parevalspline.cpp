/*
 * parevalspline.cpp
 *
 * Code generation for function 'parevalspline'
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
void parevalspline(const emxArray_real_T *coefs, const emxArray_real_T *breaks,
                   const emxArray_real_T *t, emxArray_real_T *result)
{
  emxArray_real_T *hist;
  int32_T bin;
  int32_T xind;
  emxArray_real_T *curvn;
  uint32_T outsize_idx_0;
  boolean_T guard1 = FALSE;
  int32_T exitg1;
  int32_T k;
  uint32_T low_ip1;
  uint32_T high_i;
  uint32_T mid_i;
  emxArray_real_T *unusedU0;
  emxArray_real_T *r10;
  emxInit_real_T(&hist, 1);

  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'parevalspline:4' hist = breaks; */
  bin = hist->size[0];
  hist->size[0] = breaks->size[0];
  emxEnsureCapacity((emxArray__common *)hist, bin, (int32_T)sizeof(real_T));
  xind = breaks->size[0] - 1;
  for (bin = 0; bin <= xind; bin++) {
    hist->data[bin] = breaks->data[bin];
  }

  emxInit_real_T(&curvn, 1);

  /* 'parevalspline:5' hist(end) = hist(end) + 1; */
  hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

  /* increase the last bin to include the last value */
  /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
  outsize_idx_0 = (uint32_T)t->size[0];
  bin = curvn->size[0];
  curvn->size[0] = (int32_T)outsize_idx_0;
  emxEnsureCapacity((emxArray__common *)curvn, bin, (int32_T)sizeof(real_T));
  xind = (int32_T)outsize_idx_0 - 1;
  for (bin = 0; bin <= xind; bin++) {
    curvn->data[bin] = 0.0;
  }

  guard1 = FALSE;
  if (hist->size[0] > 1) {
    xind = 0;
    do {
      exitg1 = 0;
      if (xind <= hist->size[0] - 2) {
        if (hist->data[xind + 1] < hist->data[xind]) {
          bin = curvn->size[0];
          curvn->size[0] = (int32_T)outsize_idx_0;
          emxEnsureCapacity((emxArray__common *)curvn, bin, (int32_T)sizeof
                            (real_T));
          xind = (int32_T)outsize_idx_0 - 1;
          for (bin = 0; bin <= xind; bin++) {
            curvn->data[bin] = rtNaN;
          }

          exitg1 = 1;
        } else {
          xind++;
        }
      } else {
        guard1 = TRUE;
        exitg1 = 1;
      }
    } while (exitg1 == 0U);
  } else {
    guard1 = TRUE;
  }

  if (guard1 == TRUE) {
    xind = 0;
    for (k = 0; k <= t->size[0] - 1; k++) {
      bin = 0;
      if ((!(hist->size[0] == 0)) && (!rtIsNaN(t->data[xind]))) {
        if ((t->data[xind] >= hist->data[0]) && (t->data[xind] < hist->data
             [hist->size[0] - 1])) {
          outsize_idx_0 = 1U;
          low_ip1 = 2U;
          high_i = (uint32_T)hist->size[0];
          while (high_i > low_ip1) {
            mid_i = (outsize_idx_0 >> 1U) + (high_i >> 1U);
            if (((outsize_idx_0 & 1U) == 1U) && ((high_i & 1U) == 1U)) {
              mid_i++;
            }

            if (t->data[xind] >= hist->data[(int32_T)mid_i - 1]) {
              outsize_idx_0 = mid_i;
              low_ip1 = mid_i + 1U;
            } else {
              high_i = mid_i;
            }
          }

          bin = (int32_T)outsize_idx_0;
        }

        if (t->data[xind] == hist->data[hist->size[0] - 1]) {
          bin = hist->size[0];
        }
      }

      curvn->data[xind] = (real_T)bin;
      xind++;
    }
  }

  /* 'parevalspline:9' if d == 0 */
  /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
  /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
  bin = hist->size[0];
  hist->size[0] = t->size[0];
  emxEnsureCapacity((emxArray__common *)hist, bin, (int32_T)sizeof(real_T));
  xind = t->size[0] - 1;
  for (bin = 0; bin <= xind; bin++) {
    hist->data[bin] = t->data[bin] - breaks->data[(int32_T)curvn->data[bin] - 1];
  }

  emxInit_real_T(&unusedU0, 1);
  outsize_idx_0 = (uint32_T)hist->size[0];
  bin = unusedU0->size[0];
  unusedU0->size[0] = (int32_T)outsize_idx_0;
  emxEnsureCapacity((emxArray__common *)unusedU0, bin, (int32_T)sizeof(real_T));
  bin = unusedU0->size[0];
  for (k = 0; k <= bin - 1; k++) {
    unusedU0->data[k] = rt_powd_snf(hist->data[k], 3.0);
  }

  bin = hist->size[0];
  hist->size[0] = t->size[0];
  emxEnsureCapacity((emxArray__common *)hist, bin, (int32_T)sizeof(real_T));
  xind = t->size[0] - 1;
  for (bin = 0; bin <= xind; bin++) {
    hist->data[bin] = t->data[bin] - breaks->data[(int32_T)curvn->data[bin] - 1];
  }

  emxInit_real_T(&r10, 1);
  outsize_idx_0 = (uint32_T)hist->size[0];
  bin = r10->size[0];
  r10->size[0] = (int32_T)outsize_idx_0;
  emxEnsureCapacity((emxArray__common *)r10, bin, (int32_T)sizeof(real_T));
  bin = r10->size[0];
  for (k = 0; k <= bin - 1; k++) {
    r10->data[k] = rt_powd_snf(hist->data[k], 2.0);
  }

  emxFree_real_T(&hist);
  bin = result->size[0];
  result->size[0] = curvn->size[0];
  emxEnsureCapacity((emxArray__common *)result, bin, (int32_T)sizeof(real_T));
  xind = curvn->size[0] - 1;
  for (bin = 0; bin <= xind; bin++) {
    result->data[bin] = ((coefs->data[(int32_T)curvn->data[bin] - 1] *
                          unusedU0->data[bin] + coefs->data[((int32_T)
      curvn->data[bin] + coefs->size[0]) - 1] * r10->data[bin]) + coefs->data
                         [((int32_T)curvn->data[bin] + (coefs->size[0] << 1)) -
                         1] * (t->data[bin] - breaks->data[(int32_T)curvn->
      data[bin] - 1])) + coefs->data[((int32_T)curvn->data[bin] + coefs->size[0]
      * 3) - 1];
  }

  emxFree_real_T(&r10);
  emxFree_real_T(&curvn);
  emxFree_real_T(&unusedU0);

  /* 'parevalspline:13' if d == 1 */
  /* 'parevalspline:16' if d == 2 */
}

/* End of code generation (parevalspline.cpp) */
