/*
 * localize.cpp
 *
 * Code generation for function 'localize'
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
 * function [value, distance, loccurvn, count] = localize(coefx,coefy,breaks,x0,y0,sguess,epsilon)
 */
void localize(const emxArray_real_T *coefx, const emxArray_real_T *coefy, const
              emxArray_real_T *breaks, real_T x0, real_T b_y0, real_T sguess,
              real_T epsilon, real_T *value, real_T *distance, real_T *loccurvn,
              real_T *count)
{
  real_T error1;
  real_T error2;
  real_T s1;
  real_T s3;
  real_T Ay;
  real_T xs;
  real_T ys;
  real_T Bx;
  real_T By;
  real_T Ds3;
  real_T Ax;
  real_T ss[4];
  real_T distances[4];
  int32_T ixstart;
  int32_T itmp;
  int32_T ix;
  boolean_T exitg1;
  int32_T i17;
  real_T ss_data[7];
  int32_T ss_size[1];
  int32_T i18;
  int32_T b_ss_size[1];
  real_T b_ss_data[7];

  /* UNTITLED Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'localize:5' error1 = 10; */
  error1 = 10.0;

  /* 'localize:6' error2 = 10; */
  error2 = 10.0;

  /* 'localize:8' s1 = breaks(sguess) */
  s1 = breaks->data[(int32_T)sguess - 1];

  /* 'localize:9' s3 = breaks(sguess + 1) */
  s3 = breaks->data[(int32_T)(uint32_T)sguess];

  /* 'localize:10' s2 = (s1+s3)/2 */
  *value = (breaks->data[(int32_T)sguess - 1] + breaks->data[(int32_T)(sguess +
             1.0) - 1]) / 2.0;

  /* 'localize:12' count = 0; */
  *count = 0.0;

  /* 'localize:14' while (error1 > epsilon || error2 > epsilon) && count < 50 */
  while (((error1 > epsilon) || (error2 > epsilon)) && (*count < 50.0)) {
    /* 'localize:16' if count < 7 */
    if (*count < 7.0) {
      /* 'localize:17' y23 = s2^2-s3^2; */
      /* 'localize:18' y31 = s3^2-s1^2; */
      /* 'localize:19' y12 = s1^2-s2^2; */
      /* 'localize:21' s23 = s2-s3; */
      /* 'localize:22' s31 = s3-s1; */
      /* 'localize:23' s12 = s1-s2; */
      /* 'localize:25' Ds1 = dissqrd(coefx,coefy,breaks,s1,x0,y0) */
      /* UNTITLED2 Summary of this function goes here */
      /*    Detailed explanation goes here */
      /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
      c_parevalspline(coefx, breaks, s1, &xs, &Ay);

      /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
      ys = d_parevalspline(coefy, breaks, s1);

      /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
      Bx = rt_powd_snf(xs - x0, 2.0) + rt_powd_snf(ys - b_y0, 2.0);

      /* 'localize:26' Ds2 = dissqrd(coefx,coefy,breaks,s2,x0,y0) */
      /* UNTITLED2 Summary of this function goes here */
      /*    Detailed explanation goes here */
      /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
      c_parevalspline(coefx, breaks, *value, &xs, &Ay);

      /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
      ys = d_parevalspline(coefy, breaks, *value);

      /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
      By = rt_powd_snf(xs - x0, 2.0) + rt_powd_snf(ys - b_y0, 2.0);

      /* 'localize:27' Ds3 = dissqrd(coefx,coefy,breaks,s3,x0,y0) */
      /* UNTITLED2 Summary of this function goes here */
      /*    Detailed explanation goes here */
      /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
      c_parevalspline(coefx, breaks, s3, &xs, &Ay);

      /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
      ys = d_parevalspline(coefy, breaks, s3);

      /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
      Ds3 = rt_powd_snf(xs - x0, 2.0) + rt_powd_snf(ys - b_y0, 2.0);

      /* 'localize:30' sstar = 0.5 *( (y23*Ds1 + y31*Ds2 + y12*Ds3)/... */
      /* 'localize:31'                       (s23*Ds1 + s31*Ds2 + s12*Ds3)) */
      Ax = 0.5 * ((((rt_powd_snf(*value, 2.0) - rt_powd_snf(s3, 2.0)) * Bx +
                    (rt_powd_snf(s3, 2.0) - rt_powd_snf(s1, 2.0)) * By) +
                   (rt_powd_snf(s1, 2.0) - rt_powd_snf(*value, 2.0)) * Ds3) /
                  (((*value - s3) * Bx + (s3 - s1) * By) + (s1 - *value) * Ds3));

      /* sometimes when looking s=0 it gives negative or NaN which cannot be */
      /* computed obviously. Thus change them to 0. */
      /* 'localize:35' if sstar < 0 || isnan(sstar) */
      if ((Ax < 0.0) || rtIsNaN(Ax)) {
        /* 'localize:36' sstar = 0; */
        Ax = 0.0;
      }

      /* 'localize:39' if sstar > breaks(end) */
      if (Ax > breaks->data[breaks->size[0] - 1]) {
        /* 'localize:40' sstar = breaks(end); */
        Ax = breaks->data[breaks->size[0] - 1];
      }

      /* 'localize:43' if sstar > breaks(sguess + 1) */
      if (Ax > breaks->data[(int32_T)(uint32_T)sguess]) {
        /* 'localize:44' sguess = sguess + 1 */
        sguess++;

        /* 'localize:45' s1 = breaks(sguess) */
        s1 = breaks->data[(int32_T)sguess - 1];

        /* 'localize:46' s3 = breaks(sguess + 1) */
        s3 = breaks->data[(int32_T)(uint32_T)sguess];

        /* 'localize:47' s2 = (s1+s3)/2 */
        *value = (breaks->data[(int32_T)sguess - 1] + breaks->data[(int32_T)
                  (sguess + 1.0) - 1]) / 2.0;

        /* 'localize:48' count = count + 1 */
        (*count)++;
      } else {
        /* 'localize:52' if sstar < breaks(sguess) */
        if (Ax < breaks->data[(int32_T)sguess - 1]) {
          /* 'localize:53' sguess = sguess - 1 */
          sguess--;

          /* 'localize:54' s1 = breaks(sguess) */
          s1 = breaks->data[(int32_T)sguess - 1];

          /* 'localize:55' s3 = breaks(sguess + 1) */
          s3 = breaks->data[(int32_T)sguess];

          /* 'localize:56' s2 = (s1+s3)/2 */
          *value = (breaks->data[(int32_T)sguess - 1] + breaks->data[(int32_T)
                    sguess]) / 2.0;

          /* 'localize:57' count - count + 1 */
        } else {
          /* 'localize:61' ss = [s1;s2;s3;sstar]; */
          ss[0] = s1;
          ss[1] = *value;
          ss[2] = s3;
          ss[3] = Ax;

          /* 'localize:63' Dsstar = dissqrd(coefx,coefy,breaks,sstar,x0,y0) */
          /* UNTITLED2 Summary of this function goes here */
          /*    Detailed explanation goes here */
          /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
          c_parevalspline(coefx, breaks, Ax, &xs, &Ay);

          /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
          ys = d_parevalspline(coefy, breaks, Ax);

          /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
          /* 'localize:65' distances = [Ds1;Ds2;Ds3;Dsstar]; */
          distances[0] = Bx;
          distances[1] = By;
          distances[2] = Ds3;
          distances[3] = rt_powd_snf(xs - x0, 2.0) + rt_powd_snf(ys - b_y0, 2.0);

          /* 'localize:67' [~, index] = max(distances); */
          ixstart = 1;
          Ax = distances[0];
          itmp = 0;
          if (rtIsNaN(distances[0])) {
            ix = 2;
            exitg1 = FALSE;
            while ((exitg1 == 0U) && (ix < 5)) {
              ixstart = ix;
              if (!rtIsNaN(distances[ix - 1])) {
                Ax = distances[ix - 1];
                exitg1 = TRUE;
              } else {
                ix++;
              }
            }
          }

          if (ixstart < 4) {
            while (ixstart + 1 < 5) {
              if (distances[ixstart] > Ax) {
                Ax = distances[ixstart];
                itmp = ixstart;
              }

              ixstart++;
            }
          }

          /* 'localize:69' ss = sort([ss(1:index-1);ss(index+1:end)]); */
          if (1 > itmp) {
            i17 = 0;
          } else {
            i17 = itmp;
          }

          if (itmp + 2 > 4) {
            ixstart = 0;
            ix = -1;
          } else {
            ixstart = itmp + 1;
            ix = 3;
          }

          ss_size[0] = ((i17 + ix) - ixstart) + 1;
          itmp = i17 - 1;
          for (i18 = 0; i18 <= itmp; i18++) {
            ss_data[i18] = ss[i18];
          }

          itmp = ix - ixstart;
          for (ix = 0; ix <= itmp; ix++) {
            ss_data[ix + i17] = ss[ixstart + ix];
          }

          eml_sort(ss_data, ss_size, b_ss_data, b_ss_size);

          /* 'localize:71' s1 = ss(1) */
          s1 = b_ss_data[0];

          /* 'localize:72' s2 = ss(2) */
          *value = b_ss_data[1];

          /* 'localize:73' s3 = ss(3) */
          s3 = b_ss_data[2];

          /* 'localize:75' error1 = abs(s1-s2); */
          error1 = fabs(b_ss_data[0] - b_ss_data[1]);

          /* 'localize:76' error2 = abs(s2-s3); */
          error2 = fabs(b_ss_data[1] - b_ss_data[2]);

          /* 'localize:77' count = count + 1 */
          (*count)++;
        }
      }
    } else {
      /* 'localize:80' else */
      /* 'localize:82' [xs, ~] = parevalspline(coefx,breaks,s2,0); */
      c_parevalspline(coefx, breaks, *value, &xs, &Ax);

      /* 'localize:83' [x1s, ~] = parevalspline(coefx,breaks,s2,1); */
      e_parevalspline(coefx, breaks, *value, &Ds3, &Ax);

      /* 'localize:84' [x2s, ~] = parevalspline(coefx,breaks,s2,2); */
      f_parevalspline(coefx, breaks, *value, &By, &Ax);

      /* 'localize:85' [ys, ~] = parevalspline(coefy,breaks,s2,0); */
      c_parevalspline(coefy, breaks, *value, &ys, &Ax);

      /* 'localize:86' [y1s, ~] = parevalspline(coefy,breaks,s2,1); */
      e_parevalspline(coefy, breaks, *value, &Bx, &Ax);

      /* 'localize:87' [y2s, ~] = parevalspline(coefy,breaks,s2,2); */
      f_parevalspline(coefy, breaks, *value, &Ax, &Ay);

      /* 'localize:89' D1 = 2*(-x0+xs)*x1s + 2*(-y0+ys)*y1s */
      /* 'localize:90' D2 = 2*x1s^2 + 2*y1s^2 + 2*(-x0 + xs)*x2s + 2*(-y0 + ys)*y2s */
      /* 'localize:92' sstar = s2 - D1/D2 */
      Ax = *value - (2.0 * (-x0 + xs) * Ds3 + 2.0 * (-b_y0 + ys) * Bx) / (((2.0 *
        rt_powd_snf(Ds3, 2.0) + 2.0 * rt_powd_snf(Bx, 2.0)) + 2.0 * (-x0 + xs) *
        By) + 2.0 * (-b_y0 + ys) * Ax);

      /* 'localize:94' if sstar > breaks(sguess + 1) */
      if (Ax > breaks->data[(int32_T)(uint32_T)sguess]) {
        /* 'localize:95' sguess = sguess + 1; */
        sguess++;

        /* 'localize:96' s2 = breaks(sguess); */
        *value = breaks->data[(int32_T)sguess - 1];

        /* 'localize:97' count = count + 1; */
        (*count)++;
      } else {
        /* 'localize:101' if sstar < breaks(sguess) */
        if (Ax < breaks->data[(int32_T)sguess - 1]) {
          /* 'localize:102' sguess = sguess - 1; */
          sguess--;

          /* 'localize:103' s2 = breaks(sguess); */
          *value = breaks->data[(int32_T)sguess - 1];

          /* 'localize:104' count - count + 1; */
        } else {
          /* 'localize:108' Dsstar = dissqrd(coefx,coefy,breaks,sstar,x0,y0) */
          /* UNTITLED2 Summary of this function goes here */
          /*    Detailed explanation goes here */
          /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
          /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
          /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
          /* 'localize:110' distances = [Dsstar; dissqrd(coefx,coefy,breaks,sstar,x0,y0)]; */
          /* UNTITLED2 Summary of this function goes here */
          /*    Detailed explanation goes here */
          /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
          /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
          /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
          /* 'localize:112' error1 = abs(sstar - s2); */
          error1 = fabs(Ax - *value);

          /* 'localize:113' error2 = error1; */
          error2 = error1;

          /* 'localize:115' s2 = sstar; */
          *value = Ax;

          /* 'localize:117' count = count + 1 */
          (*count)++;
        }
      }
    }
  }

  /* 'localize:121' value = s2; */
  /* disp('localize'); */
  /* 'localize:123' [distance, loccurvn] = dissqrd(coefx,coefy,breaks,s2,x0,y0) */
  /* UNTITLED2 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'dissqrd:5' [xs, curvn] = parevalspline(coefx,breaks,s,0); */
  c_parevalspline(coefx, breaks, *value, &xs, loccurvn);

  /* 'dissqrd:6' ys = parevalspline(coefy,breaks,s,0); */
  ys = d_parevalspline(coefy, breaks, *value);

  /* 'dissqrd:8' distance = (xs - x0).^2 + (ys - y0).^2; */
  /* 'localize:124' distance = sqrt(distance); */
  /* calculate whether point is to the left of right of the line. */
  /* -ve = right, +ve = left */
  /* 'localize:128' Ax = parevalspline(coefx,breaks,breaks(loccurvn),0); */
  Ax = d_parevalspline(coefx, breaks, breaks->data[(int32_T)*loccurvn - 1]);

  /* 'localize:129' Ay = parevalspline(coefy,breaks,breaks(loccurvn),0); */
  Ay = d_parevalspline(coefy, breaks, breaks->data[(int32_T)*loccurvn - 1]);

  /* 'localize:130' Bx = parevalspline(coefx,breaks,breaks(loccurvn+1),0); */
  Bx = d_parevalspline(coefx, breaks, breaks->data[(int32_T)(uint32_T)*loccurvn]);

  /* 'localize:131' By = parevalspline(coefy,breaks,breaks(loccurvn+1),0); */
  By = d_parevalspline(coefy, breaks, breaks->data[(int32_T)(uint32_T)*loccurvn]);

  /* 'localize:133' position = sign( (Bx-Ax)*(y0-Ay) - (By-Ay)*(x0-Ax) ); */
  /* 'localize:135' distance = position*distance; */
  Ax = (Bx - Ax) * (b_y0 - Ay) - (By - Ay) * (x0 - Ax);
  if (Ax < 0.0) {
    Ax = -1.0;
  } else if (Ax > 0.0) {
    Ax = 1.0;
  } else {
    if (Ax == 0.0) {
      Ax = 0.0;
    }
  }

  *distance = Ax * sqrt(rt_powd_snf(xs - x0, 2.0) + rt_powd_snf(ys - b_y0, 2.0));
}

/* End of code generation (localize.cpp) */
