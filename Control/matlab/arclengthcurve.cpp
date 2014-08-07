/*
 * arclengthcurve.cpp
 *
 * Code generation for function 'arclengthcurve'
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
#include "adaptivegaus.h"
#include "matlab_emxutil.h"
#include "cubic_spline3.h"
#include "diff.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function [scoefx, scoefy, si] = arclengthcurve(points, gran, epsilon)
 */
void arclengthcurve(const emxArray_real_T *points, real_T gran, real_T epsilon,
                    emxArray_real_T *scoefx, emxArray_real_T *scoefy,
                    emxArray_real_T *si)
{
  emxArray_real_T *b_points;
  int32_T i0;
  int32_T loop_ub;
  int32_T ar;
  emxArray_real_T *a;
  int32_T iv0[2];
  emxArray_real_T *y;
  int32_T k;
  emxArray_real_T *lengths;
  int32_T m;
  int32_T vstride;
  int32_T br;
  int32_T ia;
  emxArray_real_T *tihat;
  emxArray_real_T *ti;
  real_T totallength;
  emxArray_real_T *c_points;
  emxArray_real_T *d_points;
  emxArray_real_T *coefx;
  emxArray_real_T *coefy;
  real_T b_coefx[4];
  real_T b_coefy[4];
  real_T nsegmentsnew;
  real_T l;
  emxArray_real_T *newpoints;
  uint32_T j;
  real_T cumulength;
  real_T tleft;
  real_T tright;
  real_T tmiddle;
  real_T deltas;
  emxArray_real_T *b_newpoints;
  emxArray_real_T *c_newpoints;
  b_emxInit_real_T(&b_points, 2);

  /* UNTITLED13 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* epsilon = 1e-4; %Acceptable error in algorithms. */
  /* build spline from points using minimal tangent acceleration */
  /* 'arclengthcurve:9' ti = cumsum([0;((diff(points.').^2)*ones(2,1)).^(1/4)]); */
  i0 = b_points->size[0] * b_points->size[1];
  b_points->size[0] = points->size[1];
  b_points->size[1] = 2;
  emxEnsureCapacity((emxArray__common *)b_points, i0, (int32_T)sizeof(real_T));
  for (i0 = 0; i0 < 2; i0++) {
    loop_ub = points->size[1] - 1;
    for (ar = 0; ar <= loop_ub; ar++) {
      b_points->data[ar + b_points->size[0] * i0] = points->data[i0 +
        points->size[0] * ar];
    }
  }

  b_emxInit_real_T(&a, 2);
  diff(b_points, a);
  emxFree_real_T(&b_points);
  for (i0 = 0; i0 < 2; i0++) {
    iv0[i0] = a->size[i0];
  }

  b_emxInit_real_T(&y, 2);
  i0 = y->size[0] * y->size[1];
  y->size[0] = iv0[0];
  y->size[1] = iv0[1];
  emxEnsureCapacity((emxArray__common *)y, i0, (int32_T)sizeof(real_T));
  i0 = y->size[0] * y->size[1];
  for (k = 0; k <= i0 - 1; k++) {
    y->data[k] = rt_powd_snf(a->data[k], 2.0);
  }

  emxFree_real_T(&a);
  emxInit_real_T(&lengths, 1);
  if (y->size[1] == 1) {
    i0 = lengths->size[0];
    lengths->size[0] = y->size[0];
    emxEnsureCapacity((emxArray__common *)lengths, i0, (int32_T)sizeof(real_T));
    loop_ub = y->size[0] - 1;
    for (i0 = 0; i0 <= loop_ub; i0++) {
      lengths->data[i0] = 0.0;
      for (ar = 0; ar < 2; ar++) {
        lengths->data[i0] += y->data[i0 + y->size[0] * ar];
      }
    }
  } else {
    k = y->size[1];
    iv0[0] = y->size[0];
    iv0[1] = 1;
    i0 = lengths->size[0];
    lengths->size[0] = iv0[0];
    emxEnsureCapacity((emxArray__common *)lengths, i0, (int32_T)sizeof(real_T));
    m = y->size[0];
    ar = lengths->size[0];
    i0 = lengths->size[0];
    lengths->size[0] = ar;
    emxEnsureCapacity((emxArray__common *)lengths, i0, (int32_T)sizeof(real_T));
    loop_ub = ar - 1;
    for (i0 = 0; i0 <= loop_ub; i0++) {
      lengths->data[i0] = 0.0;
    }

    if (m == 0) {
    } else {
      ar = 0;
      while (ar <= 0) {
        for (vstride = 1; vstride <= m; vstride++) {
          lengths->data[vstride - 1] = 0.0;
        }

        ar = m;
      }

      br = 0;
      ar = 0;
      while (ar <= 0) {
        ar = 0;
        i0 = br + k;
        for (loop_ub = br + 1; loop_ub <= i0; loop_ub++) {
          ia = ar;
          for (vstride = 0; vstride + 1 <= m; vstride++) {
            ia++;
            lengths->data[vstride] += y->data[ia - 1];
          }

          ar += m;
        }

        br += k;
        ar = m;
      }
    }
  }

  emxFree_real_T(&y);
  emxInit_real_T(&tihat, 1);
  iv0[0] = lengths->size[0];
  iv0[1] = 1;
  i0 = tihat->size[0];
  tihat->size[0] = iv0[0];
  emxEnsureCapacity((emxArray__common *)tihat, i0, (int32_T)sizeof(real_T));
  i0 = tihat->size[0];
  for (k = 0; k <= i0 - 1; k++) {
    tihat->data[k] = rt_powd_snf(lengths->data[k], 0.25);
  }

  emxInit_real_T(&ti, 1);
  i0 = ti->size[0];
  ti->size[0] = 1 + tihat->size[0];
  emxEnsureCapacity((emxArray__common *)ti, i0, (int32_T)sizeof(real_T));
  ti->data[0] = 0.0;
  loop_ub = tihat->size[0] - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    ti->data[i0 + 1] = tihat->data[i0];
  }

  ar = 2;
  if (ti->size[0] != 1) {
    ar = 1;
  }

  if (ar <= 1) {
    i0 = ti->size[0];
  } else {
    i0 = 1;
  }

  if (i0 > 1) {
    vstride = 1;
    k = 1;
    while (k <= ar - 1) {
      vstride *= ti->size[0];
      k = 2;
    }

    ar = -1;
    for (loop_ub = 1; loop_ub <= vstride; loop_ub++) {
      ar++;
      ia = ar;
      totallength = ti->data[ar];
      for (k = 0; k <= i0 - 2; k++) {
        ia += vstride;
        totallength += ti->data[ia];
        ti->data[ia] = totallength;
      }
    }
  }

  emxInit_real_T(&c_points, 1);

  /* 'arclengthcurve:10' [coefx, coefy] = cubic_spline3(points(1,:)',points(2,:)',ti); */
  i0 = points->size[1];
  ar = c_points->size[0];
  c_points->size[0] = i0;
  emxEnsureCapacity((emxArray__common *)c_points, ar, (int32_T)sizeof(real_T));
  loop_ub = i0 - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    c_points->data[i0] = points->data[points->size[0] * i0];
  }

  emxInit_real_T(&d_points, 1);
  i0 = points->size[1];
  ar = d_points->size[0];
  d_points->size[0] = i0;
  emxEnsureCapacity((emxArray__common *)d_points, ar, (int32_T)sizeof(real_T));
  loop_ub = i0 - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    d_points->data[i0] = points->data[1 + points->size[0] * i0];
  }

  b_emxInit_real_T(&coefx, 2);
  b_emxInit_real_T(&coefy, 2);
  cubic_spline3(c_points, d_points, ti, coefx, coefy);

  /* 'arclengthcurve:12' nsegments = length(ti)-1; */
  /* 'arclengthcurve:13' lengths = zeros(nsegments,1); */
  i0 = lengths->size[0];
  lengths->size[0] = ti->size[0] - 1;
  emxEnsureCapacity((emxArray__common *)lengths, i0, (int32_T)sizeof(real_T));
  emxFree_real_T(&d_points);
  emxFree_real_T(&c_points);
  loop_ub = ti->size[0] - 2;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    lengths->data[i0] = 0.0;
  }

  /* use adaptive gaussian to calculate length of each segment */
  /* 'arclengthcurve:15' for n=1:nsegments */
  for (ar = 0; ar <= ti->size[0] - 2; ar++) {
    /* 'arclengthcurve:16' lengths(n) = adaptivegaus(coefx(n,:),coefy(n,:),ti(n),ti(n+1),ti(n),epsilon); */
    for (i0 = 0; i0 < 4; i0++) {
      b_coefx[i0] = coefx->data[ar + coefx->size[0] * i0];
    }

    for (i0 = 0; i0 < 4; i0++) {
      b_coefy[i0] = coefy->data[ar + coefy->size[0] * i0];
    }

    lengths->data[ar] = adaptivegaus(b_coefx, b_coefy, ti->data[ar], ti->data[ar
      + 1], ti->data[ar], epsilon);
  }

  /* find the total length of the original curve. */
  /* 'arclengthcurve:19' totallength = sum(lengths); */
  if (lengths->size[0] == 0) {
    totallength = 0.0;
  } else {
    ar = lengths->size[0];
    totallength = lengths->data[0];
    for (k = 2; k <= ar; k++) {
      totallength += lengths->data[k - 1];
    }
  }

  /* now build new curve */
  /* now compute m+1 equally spaced points on original curvne */
  /* so the length of each segment should be */
  /* 'arclengthcurve:25' nsegmentsnew = ceil(totallength/gran); */
  nsegmentsnew = ceil(totallength / gran);

  /* 'arclengthcurve:26' l = totallength/nsegmentsnew; */
  l = totallength / nsegmentsnew;

  /* 'arclengthcurve:28' index = 1; */
  ar = 0;

  /* index of original curv segments */
  /* 'arclengthcurve:29' tihat = zeros(nsegmentsnew+1,1); */
  i0 = tihat->size[0];
  tihat->size[0] = (int32_T)(nsegmentsnew + 1.0);
  emxEnsureCapacity((emxArray__common *)tihat, i0, (int32_T)sizeof(real_T));
  loop_ub = (int32_T)(nsegmentsnew + 1.0) - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    tihat->data[i0] = 0.0;
  }

  b_emxInit_real_T(&newpoints, 2);

  /* initialize tihat vector */
  /* 'arclengthcurve:30' newpoints = zeros(2,nsegmentsnew + 1); */
  i0 = newpoints->size[0] * newpoints->size[1];
  newpoints->size[0] = 2;
  newpoints->size[1] = (int32_T)(nsegmentsnew + 1.0);
  emxEnsureCapacity((emxArray__common *)newpoints, i0, (int32_T)sizeof(real_T));
  loop_ub = ((int32_T)(nsegmentsnew + 1.0) << 1) - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    newpoints->data[i0] = 0.0;
  }

  /* initialize new points matrix */
  /* 'arclengthcurve:31' newpoints(1,1) = evalspline(coefx(1,:),0,0,0); */
  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'evalspline:5' if d == 0 */
  /* 'evalspline:6' result = coef(1)*(t-tif)^3 + coef(2)*(t-tif)^2 + coef(3)*(t-tif) + coef(4); */
  /* 'evalspline:8' if d == 1 */
  /* 'evalspline:11' if d == 2 */
  newpoints->data[0] = ((coefx->data[0] * 0.0 + coefx->data[coefx->size[0]] *
    0.0) + coefx->data[coefx->size[0] << 1] * 0.0) + coefx->data[coefx->size[0] *
    3];

  /* evaluate x at 0 */
  /* 'arclengthcurve:32' newpoints(2,1) = evalspline(coefy(1,:),0,0,0); */
  /* UNTITLED16 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'evalspline:5' if d == 0 */
  /* 'evalspline:6' result = coef(1)*(t-tif)^3 + coef(2)*(t-tif)^2 + coef(3)*(t-tif) + coef(4); */
  /* 'evalspline:8' if d == 1 */
  /* 'evalspline:11' if d == 2 */
  newpoints->data[1] = ((coefy->data[0] * 0.0 + coefy->data[coefy->size[0]] *
    0.0) + coefy->data[coefy->size[0] << 1] * 0.0) + coefy->data[coefy->size[0] *
    3];

  /* evaluate y at 0 */
  /* 'arclengthcurve:33' si = zeros(nsegmentsnew+1,1); */
  i0 = si->size[0];
  si->size[0] = (int32_T)(nsegmentsnew + 1.0);
  emxEnsureCapacity((emxArray__common *)si, i0, (int32_T)sizeof(real_T));
  loop_ub = (int32_T)(nsegmentsnew + 1.0) - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    si->data[i0] = 0.0;
  }

  /* initialize si vector */
  /* start at j=2 because si,tihat at j=1 =0. */
  /* 'arclengthcurve:35' j=2; */
  j = 2U;

  /* 'arclengthcurve:36' cumulength = lengths(1); */
  cumulength = lengths->data[0];

  /* cumulative length at first iteration */
  /* 'arclengthcurve:37' searchlength = ((j-1)*l); */
  totallength = l;

  /* desired length at first iteration */
  /* 'arclengthcurve:39' while j <= nsegmentsnew + 1 */
  while ((real_T)j <= nsegmentsnew + 1.0) {
    /* 'arclengthcurve:40' if cumulength >= searchlength - epsilon */
    if (cumulength >= totallength - epsilon) {
      /* found our segment to look for tihat, now use bisection */
      /* 'arclengthcurve:42' si(j) = searchlength; */
      si->data[(int32_T)j - 1] = totallength;

      /* 'arclengthcurve:43' len = si(j) - (cumulength - lengths(index)); */
      totallength = si->data[(int32_T)j - 1] - (cumulength - lengths->data[ar]);

      /* 'arclengthcurve:44' if len < epsilon */
      if (totallength < epsilon) {
        /* 'arclengthcurve:45' tihat(j) = cumulength; */
        tihat->data[(int32_T)j - 1] = cumulength;
      } else {
        /* 'arclengthcurve:46' else */
        /* 'arclengthcurve:47' tihat(j) =  bisect(ti(index),ti(index+1),... */
        /* 'arclengthcurve:48'                 len,coefx(index,:),coefy(index,:), epsilon); */
        tleft = ti->data[ar];
        tright = ti->data[ar + 1];

        /* UNTITLED15 Summary of this function goes here */
        /*    Detailed explanation goes here */
        /* 'bisect:5' tif = tleft; */
        /* 'bisect:8' while abs(tleft - tright) > (2*epsilon) */
        while (fabs(tleft - tright) > 2.0 * epsilon) {
          /* calculate the mid point to bisect apon. */
          /* 'bisect:10' tmiddle = (tleft + tright)/2; */
          tmiddle = (tleft + tright) / 2.0;

          /* find the arc length between tleft and tmiddle */
          /* 'bisect:12' deltas = adaptivegaus(coefx,coefy,tif,tmiddle,tif,epsilon); */
          for (i0 = 0; i0 < 4; i0++) {
            b_coefx[i0] = coefx->data[ar + coefx->size[0] * i0];
          }

          for (i0 = 0; i0 < 4; i0++) {
            b_coefy[i0] = coefy->data[ar + coefy->size[0] * i0];
          }

          deltas = adaptivegaus(b_coefx, b_coefy, ti->data[ar], tmiddle,
                                ti->data[ar], epsilon);

          /* 'bisect:13' if deltas < length */
          if (deltas < totallength) {
            /* 'bisect:14' tleft = tmiddle; */
            tleft = tmiddle;
          } else {
            /* 'bisect:15' else */
            /* 'bisect:16' tright = tmiddle; */
            tright = tmiddle;
          }
        }

        /* 'bisect:20' tihat = (tleft + tright)/2; */
        tihat->data[(int32_T)j - 1] = (tleft + tright) / 2.0;
      }

      /* 'arclengthcurve:51' newpoints(1,j) = evalspline(coefx(index,:),... */
      /* 'arclengthcurve:52'             tihat(j),ti(index),0); */
      /* UNTITLED16 Summary of this function goes here */
      /*    Detailed explanation goes here */
      /* 'evalspline:5' if d == 0 */
      /* 'evalspline:6' result = coef(1)*(t-tif)^3 + coef(2)*(t-tif)^2 + coef(3)*(t-tif) + coef(4); */
      /* 'evalspline:8' if d == 1 */
      /* 'evalspline:11' if d == 2 */
      newpoints->data[newpoints->size[0] * ((int32_T)j - 1)] = ((coefx->data[ar]
        * rt_powd_snf(tihat->data[(int32_T)j - 1] - ti->data[ar], 3.0) +
        coefx->data[ar + coefx->size[0]] * rt_powd_snf(tihat->data[(int32_T)j -
        1] - ti->data[ar], 2.0)) + coefx->data[ar + (coefx->size[0] << 1)] *
        (tihat->data[(int32_T)j - 1] - ti->data[ar])) + coefx->data[ar +
        coefx->size[0] * 3];

      /* 'arclengthcurve:53' newpoints(2,j) = evalspline(coefy(index,:),... */
      /* 'arclengthcurve:54'             tihat(j),ti(index),0); */
      /* UNTITLED16 Summary of this function goes here */
      /*    Detailed explanation goes here */
      /* 'evalspline:5' if d == 0 */
      /* 'evalspline:6' result = coef(1)*(t-tif)^3 + coef(2)*(t-tif)^2 + coef(3)*(t-tif) + coef(4); */
      /* 'evalspline:8' if d == 1 */
      /* 'evalspline:11' if d == 2 */
      newpoints->data[1 + newpoints->size[0] * ((int32_T)j - 1)] = ((coefy->
        data[ar] * rt_powd_snf(tihat->data[(int32_T)j - 1] - ti->data[ar], 3.0)
        + coefy->data[ar + coefy->size[0]] * rt_powd_snf(tihat->data[(int32_T)j
        - 1] - ti->data[ar], 2.0)) + coefy->data[ar + (coefy->size[0] << 1)] *
        (tihat->data[(int32_T)j - 1] - ti->data[ar])) + coefy->data[ar +
        coefy->size[0] * 3];

      /* set up next search */
      /* 'arclengthcurve:57' j = j+1; */
      j++;

      /* 'arclengthcurve:58' searchlength = ((j-1)*l); */
      totallength = ((real_T)j - 1.0) * l;
    } else {
      /* 'arclengthcurve:59' else */
      /* tihat not in this curve index so increment index */
      /* 'arclengthcurve:61' index = index + 1; */
      ar++;

      /* 'arclengthcurve:62' cumulength = cumulength + lengths(index); */
      cumulength += lengths->data[ar];
    }
  }

  emxFree_real_T(&coefy);
  emxFree_real_T(&coefx);
  emxFree_real_T(&tihat);
  emxFree_real_T(&lengths);
  emxFree_real_T(&ti);
  emxInit_real_T(&b_newpoints, 1);

  /* 'arclengthcurve:65' [scoefx, scoefy] = cubic_spline3(newpoints(1,:)',newpoints(2,:)',si); */
  i0 = newpoints->size[1];
  ar = b_newpoints->size[0];
  b_newpoints->size[0] = i0;
  emxEnsureCapacity((emxArray__common *)b_newpoints, ar, (int32_T)sizeof(real_T));
  loop_ub = i0 - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    b_newpoints->data[i0] = newpoints->data[newpoints->size[0] * i0];
  }

  emxInit_real_T(&c_newpoints, 1);
  i0 = newpoints->size[1];
  ar = c_newpoints->size[0];
  c_newpoints->size[0] = i0;
  emxEnsureCapacity((emxArray__common *)c_newpoints, ar, (int32_T)sizeof(real_T));
  loop_ub = i0 - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    c_newpoints->data[i0] = newpoints->data[1 + newpoints->size[0] * i0];
  }

  emxFree_real_T(&newpoints);
  cubic_spline3(b_newpoints, c_newpoints, si, scoefx, scoefy);
  emxFree_real_T(&c_newpoints);
  emxFree_real_T(&b_newpoints);
}

/* End of code generation (arclengthcurve.cpp) */
