/*
 * localize.cpp
 *
 * Code generation for function 'localize'
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
#include "histc.h"
#include "matlab_emxutil.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
static void eml_sort(const real_T x_data[7], const int32_T x_size[1], real_T
                     y_data[7], int32_T y_size[1]);

/* Function Definitions */

/*
 *
 */
static void eml_sort(const real_T x_data[7], const int32_T x_size[1], real_T
                     y_data[7], int32_T y_size[1])
{
  int32_T dim;
  int32_T a;
  real_T vwork_data[7];
  int32_T vstride;
  int32_T k;
  int32_T i1;
  int32_T j;
  int32_T iidx_data[7];
  int32_T np1;
  boolean_T p;
  int32_T i2;
  int32_T idx0_data[7];
  int32_T b_j;
  int32_T pEnd;
  int32_T b_p;
  int32_T q;
  int32_T qEnd;
  int32_T kEnd;
  dim = 2;
  if (x_size[0] != 1) {
    dim = 1;
  }

  if (dim <= 1) {
    a = x_size[0] - 1;
  } else {
    a = 0;
  }

  y_size[0] = x_size[0];
  vstride = 1;
  k = 1;
  while (k <= dim - 1) {
    vstride *= x_size[0];
    k = 2;
  }

  i1 = 0;
  for (j = 1; j <= vstride; j++) {
    i1++;
    dim = i1;
    for (k = 0; k <= a; k++) {
      vwork_data[k] = x_data[dim - 1];
      dim += vstride;
    }

    np1 = (int8_T)(a + 1) + 1;
    if ((int8_T)(a + 1) == 0) {
      for (k = 1; k <= (int8_T)(a + 1); k++) {
        iidx_data[k - 1] = k;
      }
    } else {
      for (k = 1; k <= (int8_T)(a + 1); k++) {
        iidx_data[k - 1] = k;
      }

      for (k = 1; k <= (int8_T)(a + 1) - 1; k += 2) {
        if ((vwork_data[k - 1] <= vwork_data[k]) || rtIsNaN(vwork_data[k])) {
          p = TRUE;
        } else {
          p = FALSE;
        }

        if (p) {
        } else {
          iidx_data[k - 1] = k + 1;
          iidx_data[k] = k;
        }
      }

      dim = (int8_T)(a + 1) - 1;
      for (i2 = 0; i2 <= dim; i2++) {
        idx0_data[i2] = 1;
      }

      dim = 2;
      while (dim < (int8_T)(a + 1)) {
        i2 = dim << 1;
        b_j = 1;
        for (pEnd = 1 + dim; pEnd < np1; pEnd = qEnd + dim) {
          b_p = b_j;
          q = pEnd - 1;
          qEnd = b_j + i2;
          if (qEnd > np1) {
            qEnd = np1;
          }

          k = 0;
          kEnd = qEnd - b_j;
          while (k + 1 <= kEnd) {
            if ((vwork_data[iidx_data[b_p - 1] - 1] <= vwork_data[iidx_data[q] -
                 1]) || rtIsNaN(vwork_data[iidx_data[q] - 1])) {
              p = TRUE;
            } else {
              p = FALSE;
            }

            if (p) {
              idx0_data[k] = iidx_data[b_p - 1];
              b_p++;
              if (b_p == pEnd) {
                while (q + 1 < qEnd) {
                  k++;
                  idx0_data[k] = iidx_data[q];
                  q++;
                }
              }
            } else {
              idx0_data[k] = iidx_data[q];
              q++;
              if (q + 1 == qEnd) {
                while (b_p < pEnd) {
                  k++;
                  idx0_data[k] = iidx_data[b_p - 1];
                  b_p++;
                }
              }
            }

            k++;
          }

          for (k = 0; k + 1 <= kEnd; k++) {
            iidx_data[(b_j + k) - 1] = idx0_data[k];
          }

          b_j = qEnd;
        }

        dim = i2;
      }
    }

    dim = i1;
    for (k = 0; k <= a; k++) {
      y_data[dim - 1] = vwork_data[iidx_data[k] - 1];
      dim += vstride;
    }
  }
}

/*
 * function [value, distance, curvn] = localize(coefx,coefy,breaks,x0,y0,sguess,epsilon)
 */
void localize(const emxArray_real_T *coefx, const emxArray_real_T *coefy, const
              emxArray_real_T *breaks, real_T x0, real_T b_y0, real_T sguess,
              real_T epsilon, real_T *value, real_T *distance, real_T *curvn)
{
  real_T error1;
  real_T error2;
  real_T s1;
  real_T s3;
  emxArray_real_T *hist;
  emxArray_real_T *b_hist;
  emxArray_real_T *c_hist;
  real_T xs;
  real_T ys;
  real_T Bx;
  real_T ss[4];
  int32_T i9;
  int32_T ixstart;
  real_T b_curvn[4];
  real_T distances[4];
  real_T y[4];
  real_T b_y[4];
  real_T b_xs[4];
  real_T b_ys[4];
  int32_T itmp;
  int32_T ix;
  boolean_T exitg1;
  real_T ss_data[7];
  int32_T ss_size[1];
  int32_T i10;
  int32_T b_ss_size[1];
  real_T b_ss_data[7];

  /* UNTITLED Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'localize:5' error1 = 10; */
  error1 = 10.0;

  /* 'localize:6' error2 = 10; */
  error2 = 10.0;

  /* 'localize:8' s1 = breaks(sguess); */
  s1 = breaks->data[(int32_T)sguess - 1];

  /* 'localize:9' s3 = breaks(sguess + 1); */
  s3 = breaks->data[(int32_T)(uint32_T)sguess];

  /* 'localize:10' s2 = (s1+s3)/2; */
  *value = (breaks->data[(int32_T)sguess - 1] + breaks->data[(int32_T)(sguess +
             1.0) - 1]) / 2.0;

  /* 'localize:12' while error1 > epsilon || error2 > epsilon */
  emxInit_real_T(&hist, 1);
  emxInit_real_T(&b_hist, 1);
  emxInit_real_T(&c_hist, 1);
  while ((error1 > epsilon) || (error2 > epsilon)) {
    /* 'localize:14' y23 = s2^2-s3^2; */
    /* 'localize:15' y31 = s3^2-s1^2; */
    /* 'localize:16' y12 = s1^2-s2^2; */
    /* 'localize:18' s23 = s2-s3; */
    /* 'localize:19' s31 = s3-s1; */
    /* 'localize:20' s12 = s1-s2; */
    /* 'localize:22' Ds1 = dissqrd(coefx,coefy,breaks,s1,x0,y0); */
    /* UNTITLED2 Summary of this function goes here */
    /*    Detailed explanation goes here */
    /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
    c_parevalspline(coefx, breaks, s1, &xs, curvn);

    /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
    ys = d_parevalspline(coefy, breaks, s1);

    /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
    error2 = rt_powd_snf(xs - x0, 2.0) + rt_powd_snf(ys - b_y0, 2.0);

    /* 'localize:23' Ds2 = dissqrd(coefx,coefy,breaks,s2,x0,y0); */
    /* UNTITLED2 Summary of this function goes here */
    /*    Detailed explanation goes here */
    /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
    c_parevalspline(coefx, breaks, *value, &xs, curvn);

    /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
    ys = d_parevalspline(coefy, breaks, *value);

    /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
    Bx = rt_powd_snf(xs - x0, 2.0) + rt_powd_snf(ys - b_y0, 2.0);

    /* 'localize:24' Ds3 = dissqrd(coefx,coefy,breaks,s3,x0,y0); */
    /* UNTITLED2 Summary of this function goes here */
    /*    Detailed explanation goes here */
    /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
    c_parevalspline(coefx, breaks, s3, &xs, curvn);

    /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
    ys = d_parevalspline(coefy, breaks, s3);

    /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
    error1 = rt_powd_snf(xs - x0, 2.0) + rt_powd_snf(ys - b_y0, 2.0);

    /* 'localize:27' sstar = 0.5 *( (y23*Ds1 + y31*Ds2 + y12*Ds3)/... */
    /* 'localize:28'                   (s23*Ds1 + s31*Ds2 + s12*Ds3)); */
    error1 = 0.5 * ((((rt_powd_snf(*value, 2.0) - rt_powd_snf(s3, 2.0)) * error2
                      + (rt_powd_snf(s3, 2.0) - rt_powd_snf(s1, 2.0)) * Bx) +
                     (rt_powd_snf(s1, 2.0) - rt_powd_snf(*value, 2.0)) * error1)
                    / (((*value - s3) * error2 + (s3 - s1) * Bx) + (s1 - *value)
                       * error1));

    /* sometimes when looking s=0 it gives negative or NaN which cannot be */
    /* computed obviously. Thus change them to 0. */
    /* 'localize:32' if sstar < 0 || isnan(sstar) */
    if ((error1 < 0.0) || rtIsNaN(error1)) {
      /* 'localize:33' sstar = 0; */
      error1 = 0.0;
    }

    /* 'localize:36' ss = [s1;s2;s3;sstar]; */
    ss[0] = s1;
    ss[1] = *value;
    ss[2] = s3;
    ss[3] = error1;

    /* 'localize:38' distances = dissqrd(coefx,coefy,breaks,ss,x0,y0); */
    /* UNTITLED2 Summary of this function goes here */
    /*    Detailed explanation goes here */
    /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
    /* UNTITLED16 Summary of this function goes here */
    /*    Detailed explanation goes here */
    /* 'parevalspline:4' hist = breaks; */
    i9 = hist->size[0];
    hist->size[0] = breaks->size[0];
    emxEnsureCapacity((emxArray__common *)hist, i9, (int32_T)sizeof(real_T));
    ixstart = breaks->size[0] - 1;
    for (i9 = 0; i9 <= ixstart; i9++) {
      hist->data[i9] = breaks->data[i9];
    }

    /* 'parevalspline:5' hist(end) = hist(end) + 1; */
    hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

    /* increase the last bin to include the last value */
    /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
    i9 = c_hist->size[0];
    c_hist->size[0] = hist->size[0];
    emxEnsureCapacity((emxArray__common *)c_hist, i9, (int32_T)sizeof(real_T));
    ixstart = hist->size[0] - 1;
    for (i9 = 0; i9 <= ixstart; i9++) {
      c_hist->data[i9] = hist->data[i9];
    }

    c_histc(ss, c_hist, hist, b_curvn);

    /* 'parevalspline:9' if d == 0 */
    /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
    /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
    for (i9 = 0; i9 < 4; i9++) {
      distances[i9] = ss[i9] - breaks->data[(int32_T)b_curvn[i9] - 1];
    }

    for (ixstart = 0; ixstart < 4; ixstart++) {
      y[ixstart] = rt_powd_snf(distances[ixstart], 3.0);
    }

    for (i9 = 0; i9 < 4; i9++) {
      distances[i9] = ss[i9] - breaks->data[(int32_T)b_curvn[i9] - 1];
    }

    for (ixstart = 0; ixstart < 4; ixstart++) {
      b_y[ixstart] = rt_powd_snf(distances[ixstart], 2.0);
    }

    for (i9 = 0; i9 < 4; i9++) {
      b_xs[i9] = ((coefx->data[(int32_T)b_curvn[i9] - 1] * y[i9] + coefx->data
                   [((int32_T)b_curvn[i9] + coefx->size[0]) - 1] * b_y[i9]) +
                  coefx->data[((int32_T)b_curvn[i9] + (coefx->size[0] << 1)) - 1]
                  * (ss[i9] - breaks->data[(int32_T)b_curvn[i9] - 1])) +
        coefx->data[((int32_T)b_curvn[i9] + coefx->size[0] * 3) - 1];
    }

    /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
    /* UNTITLED16 Summary of this function goes here */
    /*    Detailed explanation goes here */
    /* 'parevalspline:4' hist = breaks; */
    i9 = hist->size[0];
    hist->size[0] = breaks->size[0];
    emxEnsureCapacity((emxArray__common *)hist, i9, (int32_T)sizeof(real_T));
    ixstart = breaks->size[0] - 1;
    for (i9 = 0; i9 <= ixstart; i9++) {
      hist->data[i9] = breaks->data[i9];
    }

    /* 'parevalspline:5' hist(end) = hist(end) + 1; */
    hist->data[breaks->size[0] - 1] = breaks->data[breaks->size[0] - 1] + 1.0;

    /* increase the last bin to include the last value */
    /* 'parevalspline:6' [~,curvn] = histc(t,hist); */
    i9 = b_hist->size[0];
    b_hist->size[0] = hist->size[0];
    emxEnsureCapacity((emxArray__common *)b_hist, i9, (int32_T)sizeof(real_T));
    ixstart = hist->size[0] - 1;
    for (i9 = 0; i9 <= ixstart; i9++) {
      b_hist->data[i9] = hist->data[i9];
    }

    c_histc(ss, b_hist, hist, b_curvn);

    /* 'parevalspline:9' if d == 0 */
    /* 'parevalspline:10' result = coefs(curvn,1).*(t-breaks(curvn)).^3 + coefs(curvn,2).*(t-breaks(curvn)).^2 + ... */
    /* 'parevalspline:11'         coefs(curvn,3).*(t-breaks(curvn)) + coefs(curvn,4); */
    for (i9 = 0; i9 < 4; i9++) {
      distances[i9] = ss[i9] - breaks->data[(int32_T)b_curvn[i9] - 1];
    }

    for (ixstart = 0; ixstart < 4; ixstart++) {
      y[ixstart] = rt_powd_snf(distances[ixstart], 3.0);
    }

    for (i9 = 0; i9 < 4; i9++) {
      distances[i9] = ss[i9] - breaks->data[(int32_T)b_curvn[i9] - 1];
    }

    for (ixstart = 0; ixstart < 4; ixstart++) {
      b_y[ixstart] = rt_powd_snf(distances[ixstart], 2.0);
    }

    for (i9 = 0; i9 < 4; i9++) {
      b_ys[i9] = ((coefy->data[(int32_T)b_curvn[i9] - 1] * y[i9] + coefy->data
                   [((int32_T)b_curvn[i9] + coefy->size[0]) - 1] * b_y[i9]) +
                  coefy->data[((int32_T)b_curvn[i9] + (coefy->size[0] << 1)) - 1]
                  * (ss[i9] - breaks->data[(int32_T)b_curvn[i9] - 1])) +
        coefy->data[((int32_T)b_curvn[i9] + coefy->size[0] * 3) - 1];
    }

    /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
    for (i9 = 0; i9 < 4; i9++) {
      distances[i9] = rt_powd_snf(b_xs[i9] - x0, 2.0) + rt_powd_snf(b_ys[i9] -
        b_y0, 2.0);
    }

    /* 'localize:40' [~, index] = max(distances); */
    ixstart = 1;
    error1 = distances[0];
    itmp = 0;
    if (rtIsNaN(distances[0])) {
      ix = 2;
      exitg1 = FALSE;
      while ((exitg1 == 0U) && (ix < 5)) {
        ixstart = ix;
        if (!rtIsNaN(distances[ix - 1])) {
          error1 = distances[ix - 1];
          exitg1 = TRUE;
        } else {
          ix++;
        }
      }
    }

    if (ixstart < 4) {
      while (ixstart + 1 < 5) {
        if (distances[ixstart] > error1) {
          error1 = distances[ixstart];
          itmp = ixstart;
        }

        ixstart++;
      }
    }

    /* 'localize:42' ss = sort([ss(1:index-1);ss(index+1:end)]); */
    if (1 > itmp) {
      i9 = 0;
    } else {
      i9 = itmp;
    }

    if (itmp + 2 > 4) {
      ix = 0;
      itmp = -1;
    } else {
      ix = itmp + 1;
      itmp = 3;
    }

    ss_size[0] = ((i9 + itmp) - ix) + 1;
    ixstart = i9 - 1;
    for (i10 = 0; i10 <= ixstart; i10++) {
      ss_data[i10] = ss[i10];
    }

    ixstart = itmp - ix;
    for (itmp = 0; itmp <= ixstart; itmp++) {
      ss_data[itmp + i9] = ss[ix + itmp];
    }

    eml_sort(ss_data, ss_size, b_ss_data, b_ss_size);

    /* 'localize:44' s1 = ss(1); */
    s1 = b_ss_data[0];

    /* 'localize:45' s2 = ss(2); */
    *value = b_ss_data[1];

    /* 'localize:46' s3 = ss(3); */
    s3 = b_ss_data[2];

    /* 'localize:48' error1 = abs(s1-s2); */
    error1 = fabs(b_ss_data[0] - b_ss_data[1]);

    /* 'localize:49' error2 = abs(s2-s3); */
    error2 = fabs(b_ss_data[1] - b_ss_data[2]);
  }

  emxFree_real_T(&c_hist);
  emxFree_real_T(&b_hist);
  emxFree_real_T(&hist);

  /* 'localize:52' value = s2; */
  /* 'localize:53' [distance, curvn] = dissqrd(coefx,coefy,breaks,s2,x0,y0); */
  /* UNTITLED2 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
  c_parevalspline(coefx, breaks, *value, &xs, curvn);

  /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
  ys = d_parevalspline(coefy, breaks, *value);

  /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
  /* 'localize:54' distance = sqrt(distance); */
  /* calculate whether point is to the left of right of the line. */
  /* -ve = right, +ve = left */
  /* 'localize:58' Ax = parevalspline(coefx,breaks,breaks(curvn),0); */
  error1 = d_parevalspline(coefx, breaks, breaks->data[(int32_T)*curvn - 1]);

  /* 'localize:59' Ay = parevalspline(coefy,breaks,breaks(curvn),0); */
  error2 = d_parevalspline(coefy, breaks, breaks->data[(int32_T)*curvn - 1]);

  /* 'localize:60' Bx = parevalspline(coefx,breaks,breaks(curvn+1),0); */
  Bx = d_parevalspline(coefx, breaks, breaks->data[(int32_T)(uint32_T)*curvn]);

  /* 'localize:61' By = parevalspline(coefy,breaks,breaks(curvn+1),0); */
  s1 = d_parevalspline(coefy, breaks, breaks->data[(int32_T)(uint32_T)*curvn]);

  /* 'localize:63' position = sign( (Bx-Ax)*(y0-Ay) - (By-Ay)*(x0-Ax) ); */
  /* 'localize:65' distance = position*distance; */
  error1 = (Bx - error1) * (b_y0 - error2) - (s1 - error2) * (x0 - error1);
  if (error1 < 0.0) {
    error1 = -1.0;
  } else if (error1 > 0.0) {
    error1 = 1.0;
  } else {
    if (error1 == 0.0) {
      error1 = 0.0;
    }
  }

  *distance = error1 * sqrt(rt_powd_snf(xs - x0, 2.0) + rt_powd_snf(ys - b_y0,
    2.0));
}

/* End of code generation (localize.cpp) */
