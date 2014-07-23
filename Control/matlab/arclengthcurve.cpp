/*
 * arclengthcurve.cpp
 *
 * Code generation for function 'arclengthcurve'
 *
 * C source code generated on: Wed Jul 23 14:27:34 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "builddetailedbf.h"
#include "adaptivegaus.h"
#include "matlab_emxutil.h"
#include "cubic_spline3.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function [scoefx, scoefy, si] = arclengthcurve(points, nsegmentsnew)
 */
void arclengthcurve(const emxArray_real_T *points, real_T nsegmentsnew,
                    emxArray_real_T *scoefx, emxArray_real_T *scoefy,
                    emxArray_real_T *si)
{
  emxArray_real_T *x;
  int32_T i0;
  int32_T ixStart;
  int32_T xNext;
  emxArray_real_T *y;
  int32_T newDimSize;
  uint32_T ySize[2];
  emxArray_real_T *b_y1;
  int32_T iyStart;
  int32_T ar;
  int32_T ixLead;
  int32_T iyLead;
  real_T work_data_idx_0;
  int32_T m;
  real_T tmp1;
  real_T tmp2;
  emxArray_real_T *b_y;
  emxArray_real_T *lengths;
  emxArray_real_T *tihat;
  emxArray_real_T *ti;
  emxArray_real_T *b_points;
  emxArray_real_T *c_points;
  emxArray_real_T *coefx;
  emxArray_real_T *coefy;
  real_T b_coefx[4];
  real_T b_coefy[4];
  real_T l;
  emxArray_real_T *newpoints;
  uint32_T j;
  real_T cumulength;
  real_T tmiddle;
  real_T deltas;
  emxArray_real_T *b_newpoints;
  emxArray_real_T *c_newpoints;
  b_emxInit_real_T(&x, 2);

  /* UNTITLED13 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'arclengthcurve:6' epsilon = 1e-4; */
  /* Acceptable error in algorithms. */
  /* neqpoints = c; %number of points to split the arc length curve into. */
  /* build spline from points using minimal tangent acceleration */
  /* 'arclengthcurve:10' ti = cumsum([0;((diff(points.').^2)*ones(2,1)).^(1/4)]); */
  i0 = x->size[0] * x->size[1];
  x->size[0] = points->size[1];
  x->size[1] = 2;
  emxEnsureCapacity((emxArray__common *)x, i0, (int32_T)sizeof(real_T));
  for (i0 = 0; i0 < 2; i0++) {
    ixStart = points->size[1] - 1;
    for (xNext = 0; xNext <= ixStart; xNext++) {
      x->data[xNext + x->size[0] * i0] = points->data[i0 + points->size[0] *
        xNext];
    }
  }

  b_emxInit_real_T(&y, 2);
  if (x->size[0] == 0) {
    i0 = y->size[0] * y->size[1];
    y->size[0] = 0;
    y->size[1] = 2;
    emxEnsureCapacity((emxArray__common *)y, i0, (int32_T)sizeof(real_T));
  } else {
    xNext = x->size[0] - 1;
    if (xNext <= 1) {
    } else {
      xNext = 1;
    }

    if (xNext < 1) {
      i0 = y->size[0] * y->size[1];
      y->size[0] = 0;
      y->size[1] = 0;
      emxEnsureCapacity((emxArray__common *)y, i0, (int32_T)sizeof(real_T));
    } else {
      newDimSize = x->size[0] - 1;
      for (i0 = 0; i0 < 2; i0++) {
        ySize[i0] = (uint32_T)x->size[i0];
      }

      b_emxInit_real_T(&b_y1, 2);
      ySize[0] = (uint32_T)newDimSize;
      i0 = b_y1->size[0] * b_y1->size[1];
      b_y1->size[0] = (int32_T)ySize[0];
      b_y1->size[1] = 2;
      emxEnsureCapacity((emxArray__common *)b_y1, i0, (int32_T)sizeof(real_T));
      xNext = x->size[0];
      ixStart = 0;
      iyStart = 1;
      for (ar = 0; ar < 2; ar++) {
        ixLead = ixStart;
        iyLead = iyStart;
        work_data_idx_0 = x->data[ixStart];
        for (m = 2; m <= x->size[0]; m++) {
          tmp1 = x->data[ixLead + 1];
          tmp2 = work_data_idx_0;
          work_data_idx_0 = tmp1;
          tmp1 -= tmp2;
          ixLead++;
          b_y1->data[iyLead - 1] = tmp1;
          iyLead++;
        }

        ixStart += xNext;
        iyStart += newDimSize;
      }

      i0 = y->size[0] * y->size[1];
      y->size[0] = b_y1->size[0];
      y->size[1] = 2;
      emxEnsureCapacity((emxArray__common *)y, i0, (int32_T)sizeof(real_T));
      ixStart = b_y1->size[0] * b_y1->size[1] - 1;
      for (i0 = 0; i0 <= ixStart; i0++) {
        y->data[i0] = b_y1->data[i0];
      }

      emxFree_real_T(&b_y1);
    }
  }

  emxFree_real_T(&x);
  for (i0 = 0; i0 < 2; i0++) {
    ySize[i0] = (uint32_T)y->size[i0];
  }

  b_emxInit_real_T(&b_y, 2);
  i0 = b_y->size[0] * b_y->size[1];
  b_y->size[0] = (int32_T)ySize[0];
  b_y->size[1] = (int32_T)ySize[1];
  emxEnsureCapacity((emxArray__common *)b_y, i0, (int32_T)sizeof(real_T));
  i0 = b_y->size[0] * b_y->size[1];
  for (iyLead = 0; iyLead <= i0 - 1; iyLead++) {
    b_y->data[iyLead] = rt_powd_snf(y->data[iyLead], 2.0);
  }

  emxFree_real_T(&y);
  emxInit_real_T(&lengths, 1);
  if (b_y->size[1] == 1) {
    i0 = lengths->size[0];
    lengths->size[0] = b_y->size[0];
    emxEnsureCapacity((emxArray__common *)lengths, i0, (int32_T)sizeof(real_T));
    ixStart = b_y->size[0] - 1;
    for (i0 = 0; i0 <= ixStart; i0++) {
      lengths->data[i0] = 0.0;
      for (xNext = 0; xNext < 2; xNext++) {
        lengths->data[i0] += b_y->data[i0 + b_y->size[0] * xNext];
      }
    }
  } else {
    iyLead = b_y->size[1];
    ySize[0] = (uint32_T)b_y->size[0];
    ySize[1] = 1U;
    i0 = lengths->size[0];
    lengths->size[0] = (int32_T)ySize[0];
    emxEnsureCapacity((emxArray__common *)lengths, i0, (int32_T)sizeof(real_T));
    m = b_y->size[0];
    xNext = lengths->size[0];
    i0 = lengths->size[0];
    lengths->size[0] = xNext;
    emxEnsureCapacity((emxArray__common *)lengths, i0, (int32_T)sizeof(real_T));
    ixStart = xNext - 1;
    for (i0 = 0; i0 <= ixStart; i0++) {
      lengths->data[i0] = 0.0;
    }

    if (m == 0) {
    } else {
      xNext = 0;
      while (xNext <= 0) {
        for (iyStart = 1; iyStart <= m; iyStart++) {
          lengths->data[iyStart - 1] = 0.0;
        }

        xNext = m;
      }

      ixLead = 0;
      xNext = 0;
      while (xNext <= 0) {
        ar = 0;
        i0 = ixLead + iyLead;
        for (xNext = ixLead + 1; xNext <= i0; xNext++) {
          ixStart = ar;
          for (iyStart = 0; iyStart + 1 <= m; iyStart++) {
            ixStart++;
            lengths->data[iyStart] += b_y->data[ixStart - 1];
          }

          ar += m;
        }

        ixLead += iyLead;
        xNext = m;
      }
    }
  }

  emxFree_real_T(&b_y);
  emxInit_real_T(&tihat, 1);
  ySize[0] = (uint32_T)lengths->size[0];
  ySize[1] = 1U;
  i0 = tihat->size[0];
  tihat->size[0] = (int32_T)ySize[0];
  emxEnsureCapacity((emxArray__common *)tihat, i0, (int32_T)sizeof(real_T));
  i0 = tihat->size[0];
  for (iyLead = 0; iyLead <= i0 - 1; iyLead++) {
    tihat->data[iyLead] = rt_powd_snf(lengths->data[iyLead], 0.25);
  }

  emxInit_real_T(&ti, 1);
  i0 = ti->size[0];
  ti->size[0] = 1 + tihat->size[0];
  emxEnsureCapacity((emxArray__common *)ti, i0, (int32_T)sizeof(real_T));
  ti->data[0] = 0.0;
  ixStart = tihat->size[0] - 1;
  for (i0 = 0; i0 <= ixStart; i0++) {
    ti->data[i0 + 1] = tihat->data[i0];
  }

  xNext = 2;
  if (ti->size[0] != 1) {
    xNext = 1;
  }

  if (xNext <= 1) {
    i0 = ti->size[0];
  } else {
    i0 = 1;
  }

  if (i0 > 1) {
    ar = 1;
    iyLead = 1;
    while (iyLead <= xNext - 1) {
      ar *= ti->size[0];
      iyLead = 2;
    }

    xNext = -1;
    for (ixStart = 1; ixStart <= ar; ixStart++) {
      xNext++;
      iyStart = xNext;
      work_data_idx_0 = ti->data[xNext];
      for (iyLead = 0; iyLead <= i0 - 2; iyLead++) {
        iyStart += ar;
        work_data_idx_0 += ti->data[iyStart];
        ti->data[iyStart] = work_data_idx_0;
      }
    }
  }

  emxInit_real_T(&b_points, 1);

  /* 'arclengthcurve:11' [coefx, coefy] = cubic_spline3(points(1,:)',points(2,:)',ti); */
  i0 = points->size[1];
  xNext = b_points->size[0];
  b_points->size[0] = i0;
  emxEnsureCapacity((emxArray__common *)b_points, xNext, (int32_T)sizeof(real_T));
  ixStart = i0 - 1;
  for (i0 = 0; i0 <= ixStart; i0++) {
    b_points->data[i0] = points->data[points->size[0] * i0];
  }

  emxInit_real_T(&c_points, 1);
  i0 = points->size[1];
  xNext = c_points->size[0];
  c_points->size[0] = i0;
  emxEnsureCapacity((emxArray__common *)c_points, xNext, (int32_T)sizeof(real_T));
  ixStart = i0 - 1;
  for (i0 = 0; i0 <= ixStart; i0++) {
    c_points->data[i0] = points->data[1 + points->size[0] * i0];
  }

  b_emxInit_real_T(&coefx, 2);
  b_emxInit_real_T(&coefy, 2);
  cubic_spline3(b_points, c_points, ti, coefx, coefy);

  /* 'arclengthcurve:13' nsegments = length(ti)-1; */
  /* 'arclengthcurve:14' lengths = zeros(nsegments,1); */
  i0 = lengths->size[0];
  lengths->size[0] = ti->size[0] - 1;
  emxEnsureCapacity((emxArray__common *)lengths, i0, (int32_T)sizeof(real_T));
  emxFree_real_T(&c_points);
  emxFree_real_T(&b_points);
  ixStart = ti->size[0] - 2;
  for (i0 = 0; i0 <= ixStart; i0++) {
    lengths->data[i0] = 0.0;
  }

  /* use adaptive gaussian to calculate length of each segment */
  /* 'arclengthcurve:16' for n=1:nsegments */
  for (xNext = 0; xNext <= ti->size[0] - 2; xNext++) {
    /* 'arclengthcurve:17' lengths(n) = adaptivegaus(coefx(n,:),coefy(n,:),ti(n),ti(n+1),ti(n),epsilon); */
    for (i0 = 0; i0 < 4; i0++) {
      b_coefx[i0] = coefx->data[xNext + coefx->size[0] * i0];
    }

    for (i0 = 0; i0 < 4; i0++) {
      b_coefy[i0] = coefy->data[xNext + coefy->size[0] * i0];
    }

    lengths->data[xNext] = adaptivegaus(b_coefx, b_coefy, ti->data[xNext],
      ti->data[xNext + 1], ti->data[xNext], 0.0001);
  }

  /* find the total length of the original curve. */
  /* 'arclengthcurve:20' totallength = sum(lengths); */
  if (lengths->size[0] == 0) {
    work_data_idx_0 = 0.0;
  } else {
    xNext = lengths->size[0];
    work_data_idx_0 = lengths->data[0];
    for (iyLead = 2; iyLead <= xNext; iyLead++) {
      work_data_idx_0 += lengths->data[iyLead - 1];
    }
  }

  /* now build new curve */
  /* nsegmentsnew = 30; %number of new segments */
  /* now compute m+1 equally spaced points on original curvne */
  /* so the length of each segment should be */
  /* 'arclengthcurve:28' l = totallength/nsegmentsnew; */
  l = work_data_idx_0 / nsegmentsnew;

  /* 'arclengthcurve:30' index = 1; */
  xNext = 0;

  /* index of original curv segments */
  /* 'arclengthcurve:31' tihat = zeros(nsegmentsnew+1,1); */
  i0 = tihat->size[0];
  tihat->size[0] = (int32_T)(nsegmentsnew + 1.0);
  emxEnsureCapacity((emxArray__common *)tihat, i0, (int32_T)sizeof(real_T));
  ixStart = (int32_T)(nsegmentsnew + 1.0) - 1;
  for (i0 = 0; i0 <= ixStart; i0++) {
    tihat->data[i0] = 0.0;
  }

  b_emxInit_real_T(&newpoints, 2);

  /* initialize tihat vector */
  /* 'arclengthcurve:32' newpoints = zeros(2,nsegmentsnew + 1); */
  i0 = newpoints->size[0] * newpoints->size[1];
  newpoints->size[0] = 2;
  newpoints->size[1] = (int32_T)(nsegmentsnew + 1.0);
  emxEnsureCapacity((emxArray__common *)newpoints, i0, (int32_T)sizeof(real_T));
  ixStart = ((int32_T)(nsegmentsnew + 1.0) << 1) - 1;
  for (i0 = 0; i0 <= ixStart; i0++) {
    newpoints->data[i0] = 0.0;
  }

  /* initialize new points matrix */
  /* 'arclengthcurve:33' newpoints(1,1) = evalspline(coefx(1,:),0,0,0); */
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
  /* 'arclengthcurve:34' newpoints(2,1) = evalspline(coefy(1,:),0,0,0); */
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
  /* 'arclengthcurve:35' si = zeros(nsegmentsnew+1,1); */
  i0 = si->size[0];
  si->size[0] = (int32_T)(nsegmentsnew + 1.0);
  emxEnsureCapacity((emxArray__common *)si, i0, (int32_T)sizeof(real_T));
  ixStart = (int32_T)(nsegmentsnew + 1.0) - 1;
  for (i0 = 0; i0 <= ixStart; i0++) {
    si->data[i0] = 0.0;
  }

  /* initialize si vector */
  /* start at j=2 because si,tihat at j=1 =0. */
  /* 'arclengthcurve:37' j=2; */
  j = 2U;

  /* 'arclengthcurve:38' cumulength = lengths(1); */
  cumulength = lengths->data[0];

  /* cumulative length at first iteration */
  /* 'arclengthcurve:39' searchlength = ((j-1)*l); */
  work_data_idx_0 = l;

  /* desired length at first iteration */
  /* 'arclengthcurve:41' while j <= nsegmentsnew + 1 */
  while ((real_T)j <= nsegmentsnew + 1.0) {
    /* 'arclengthcurve:42' if cumulength >= searchlength - epsilon */
    if (cumulength >= work_data_idx_0 - 0.0001) {
      /* found our segment to look for tihat, now use bisection */
      /* 'arclengthcurve:44' si(j) = searchlength; */
      si->data[(int32_T)j - 1] = work_data_idx_0;

      /* 'arclengthcurve:45' len = si(j) - (cumulength - lengths(index)); */
      work_data_idx_0 = si->data[(int32_T)j - 1] - (cumulength - lengths->
        data[xNext]);

      /* 'arclengthcurve:46' if len < epsilon */
      if (work_data_idx_0 < 0.0001) {
        /* 'arclengthcurve:47' tihat(j) = cumulength; */
        tihat->data[(int32_T)j - 1] = cumulength;
      } else {
        /* 'arclengthcurve:48' else */
        /* 'arclengthcurve:49' tihat(j) =  bisect(ti(index),ti(index+1),... */
        /* 'arclengthcurve:50'                 len,coefx(index,:),coefy(index,:), epsilon); */
        tmp1 = ti->data[xNext];
        tmp2 = ti->data[xNext + 1];

        /* UNTITLED15 Summary of this function goes here */
        /*    Detailed explanation goes here */
        /* 'bisect:5' tif = tleft; */
        /* 'bisect:8' while abs(tleft - tright) > (2*epsilon) */
        while (fabs(tmp1 - tmp2) > 0.0002) {
          /* calculate the mid point to bisect apon. */
          /* 'bisect:10' tmiddle = (tleft + tright)/2; */
          tmiddle = (tmp1 + tmp2) / 2.0;

          /* find the arc length between tleft and tmiddle */
          /* 'bisect:12' deltas = adaptivegaus(coefx,coefy,tif,tmiddle,tif,epsilon); */
          for (i0 = 0; i0 < 4; i0++) {
            b_coefx[i0] = coefx->data[xNext + coefx->size[0] * i0];
          }

          for (i0 = 0; i0 < 4; i0++) {
            b_coefy[i0] = coefy->data[xNext + coefy->size[0] * i0];
          }

          deltas = adaptivegaus(b_coefx, b_coefy, ti->data[xNext], tmiddle,
                                ti->data[xNext], 0.0001);

          /* 'bisect:13' if deltas < length */
          if (deltas < work_data_idx_0) {
            /* 'bisect:14' tleft = tmiddle; */
            tmp1 = tmiddle;
          } else {
            /* 'bisect:15' else */
            /* 'bisect:16' tright = tmiddle; */
            tmp2 = tmiddle;
          }
        }

        /* 'bisect:20' tihat = (tleft + tright)/2; */
        tihat->data[(int32_T)j - 1] = (tmp1 + tmp2) / 2.0;
      }

      /* 'arclengthcurve:53' newpoints(1,j) = evalspline(coefx(index,:),... */
      /* 'arclengthcurve:54'             tihat(j),ti(index),0); */
      /* UNTITLED16 Summary of this function goes here */
      /*    Detailed explanation goes here */
      /* 'evalspline:5' if d == 0 */
      /* 'evalspline:6' result = coef(1)*(t-tif)^3 + coef(2)*(t-tif)^2 + coef(3)*(t-tif) + coef(4); */
      /* 'evalspline:8' if d == 1 */
      /* 'evalspline:11' if d == 2 */
      newpoints->data[newpoints->size[0] * ((int32_T)j - 1)] = ((coefx->
        data[xNext] * rt_powd_snf(tihat->data[(int32_T)j - 1] - ti->data[xNext],
        3.0) + coefx->data[xNext + coefx->size[0]] * rt_powd_snf(tihat->data
        [(int32_T)j - 1] - ti->data[xNext], 2.0)) + coefx->data[xNext +
        (coefx->size[0] << 1)] * (tihat->data[(int32_T)j - 1] - ti->data[xNext]))
        + coefx->data[xNext + coefx->size[0] * 3];

      /* 'arclengthcurve:55' newpoints(2,j) = evalspline(coefy(index,:),... */
      /* 'arclengthcurve:56'             tihat(j),ti(index),0); */
      /* UNTITLED16 Summary of this function goes here */
      /*    Detailed explanation goes here */
      /* 'evalspline:5' if d == 0 */
      /* 'evalspline:6' result = coef(1)*(t-tif)^3 + coef(2)*(t-tif)^2 + coef(3)*(t-tif) + coef(4); */
      /* 'evalspline:8' if d == 1 */
      /* 'evalspline:11' if d == 2 */
      newpoints->data[1 + newpoints->size[0] * ((int32_T)j - 1)] = ((coefy->
        data[xNext] * rt_powd_snf(tihat->data[(int32_T)j - 1] - ti->data[xNext],
        3.0) + coefy->data[xNext + coefy->size[0]] * rt_powd_snf(tihat->data
        [(int32_T)j - 1] - ti->data[xNext], 2.0)) + coefy->data[xNext +
        (coefy->size[0] << 1)] * (tihat->data[(int32_T)j - 1] - ti->data[xNext]))
        + coefy->data[xNext + coefy->size[0] * 3];

      /* set up next search */
      /* 'arclengthcurve:59' j = j+1; */
      j++;

      /* 'arclengthcurve:60' searchlength = ((j-1)*l); */
      work_data_idx_0 = ((real_T)j - 1.0) * l;
    } else {
      /* 'arclengthcurve:61' else */
      /* tihat not in this curve index so increment index */
      /* 'arclengthcurve:63' index = index + 1; */
      xNext++;

      /* 'arclengthcurve:64' cumulength = cumulength + lengths(index); */
      cumulength += lengths->data[xNext];
    }
  }

  emxFree_real_T(&coefy);
  emxFree_real_T(&coefx);
  emxFree_real_T(&tihat);
  emxFree_real_T(&lengths);
  emxFree_real_T(&ti);
  emxInit_real_T(&b_newpoints, 1);

  /* 'arclengthcurve:67' [scoefx, scoefy] = cubic_spline3(newpoints(1,:)',newpoints(2,:)',si); */
  i0 = newpoints->size[1];
  xNext = b_newpoints->size[0];
  b_newpoints->size[0] = i0;
  emxEnsureCapacity((emxArray__common *)b_newpoints, xNext, (int32_T)sizeof
                    (real_T));
  ixStart = i0 - 1;
  for (i0 = 0; i0 <= ixStart; i0++) {
    b_newpoints->data[i0] = newpoints->data[newpoints->size[0] * i0];
  }

  emxInit_real_T(&c_newpoints, 1);
  i0 = newpoints->size[1];
  xNext = c_newpoints->size[0];
  c_newpoints->size[0] = i0;
  emxEnsureCapacity((emxArray__common *)c_newpoints, xNext, (int32_T)sizeof
                    (real_T));
  ixStart = i0 - 1;
  for (i0 = 0; i0 <= ixStart; i0++) {
    c_newpoints->data[i0] = newpoints->data[1 + newpoints->size[0] * i0];
  }

  emxFree_real_T(&newpoints);
  cubic_spline3(b_newpoints, c_newpoints, si, scoefx, scoefy);
  emxFree_real_T(&c_newpoints);
  emxFree_real_T(&b_newpoints);
}

/* End of code generation (arclengthcurve.cpp) */
