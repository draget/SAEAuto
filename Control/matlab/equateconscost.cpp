/*
 * equateconscost.cpp
 *
 * Code generation for function 'equateconscost'
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
#include "ismember.h"
#include "rdivide.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
static void eml_li_find(const emxArray_boolean_T *x, emxArray_int32_T *y);

/* Function Definitions */

/*
 *
 */
static void eml_li_find(const emxArray_boolean_T *x, emxArray_int32_T *y)
{
  int32_T n;
  int32_T k;
  int32_T i;
  int32_T j;
  n = x->size[0];
  k = 0;
  for (i = 1; i <= n; i++) {
    if (x->data[i - 1]) {
      k++;
    }
  }

  j = y->size[0];
  y->size[0] = k;
  emxEnsureCapacity((emxArray__common *)y, j, (int32_T)sizeof(int32_T));
  j = 0;
  for (i = 1; i <= n; i++) {
    if (x->data[i - 1]) {
      y->data[j] = i;
      j++;
    }
  }
}

/*
 * function pathconscost = equateconscost(previouspathq, pathqi, mans)
 */
void equateconscost(emxArray_real_T *previouspathq, const emxArray_real_T
                    *pathqi, emxArray_real_T *mans, emxArray_real_T
                    *pathconscost)
{
  int32_T ix;
  int32_T ib;
  emxArray_real_T *commonpreviousq;
  int32_T k;
  int32_T vlen;
  emxArray_real_T *x;
  emxArray_real_T *r24;
  emxArray_real_T *b_previouspathq;
  emxArray_boolean_T *matchpaths;
  emxArray_real_T *c_previouspathq;
  emxArray_boolean_T *matchpathsprev;
  emxArray_real_T *commonpathqi;
  emxArray_int32_T *r25;
  int32_T ia;
  int32_T mv[2];
  int32_T b_commonpreviousq[2];
  int32_T outsize[2];
  emxArray_real_T *b;
  uint32_T sz[2];
  int32_T iy;
  int32_T i;
  real_T s;

  /* UNTITLED3 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'equateconscost:5' [r, c] = size(previouspathq); */
  /* 'equateconscost:6' if r > 0 */
  if (previouspathq->size[0] > 0) {
    /* 'equateconscost:7' mans = round(mans.*1000)./1000; */
    ix = mans->size[0];
    mans->size[0] = mans->size[0];
    emxEnsureCapacity((emxArray__common *)mans, ix, (int32_T)sizeof(real_T));
    ib = mans->size[0] - 1;
    for (ix = 0; ix <= ib; ix++) {
      mans->data[ix] *= 1000.0;
    }

    emxInit_real_T(&commonpreviousq, 1);
    ix = commonpreviousq->size[0];
    commonpreviousq->size[0] = mans->size[0];
    emxEnsureCapacity((emxArray__common *)commonpreviousq, ix, (int32_T)sizeof
                      (real_T));
    ib = mans->size[0] - 1;
    for (ix = 0; ix <= ib; ix++) {
      commonpreviousq->data[ix] = mans->data[ix];
    }

    for (k = 0; k <= mans->size[0] - 1; k++) {
      commonpreviousq->data[k] = rt_roundd_snf(commonpreviousq->data[k]);
    }

    b_rdivide(commonpreviousq, 1000.0, mans);

    /* 'equateconscost:8' previouspathq(:,2) = round(previouspathq(:,2).*1000)./1000; */
    ix = previouspathq->size[0];
    vlen = commonpreviousq->size[0];
    commonpreviousq->size[0] = ix;
    emxEnsureCapacity((emxArray__common *)commonpreviousq, vlen, (int32_T)sizeof
                      (real_T));
    ib = ix - 1;
    for (ix = 0; ix <= ib; ix++) {
      commonpreviousq->data[ix] = previouspathq->data[ix + previouspathq->size[0]]
        * 1000.0;
    }

    emxInit_real_T(&x, 1);
    ix = x->size[0];
    x->size[0] = commonpreviousq->size[0];
    emxEnsureCapacity((emxArray__common *)x, ix, (int32_T)sizeof(real_T));
    ib = commonpreviousq->size[0] - 1;
    for (ix = 0; ix <= ib; ix++) {
      x->data[ix] = commonpreviousq->data[ix];
    }

    for (k = 0; k <= commonpreviousq->size[0] - 1; k++) {
      x->data[k] = rt_roundd_snf(x->data[k]);
    }

    emxInit_real_T(&r24, 1);
    b_rdivide(x, 1000.0, r24);
    emxFree_real_T(&x);
    ib = r24->size[0] - 1;
    for (ix = 0; ix <= ib; ix++) {
      previouspathq->data[ix + previouspathq->size[0]] = r24->data[ix];
    }

    emxFree_real_T(&r24);
    emxInit_real_T(&b_previouspathq, 1);

    /* 'equateconscost:10' matchpaths = ismember(mans, previouspathq(:,2)) */
    ix = previouspathq->size[0];
    vlen = b_previouspathq->size[0];
    b_previouspathq->size[0] = ix;
    emxEnsureCapacity((emxArray__common *)b_previouspathq, vlen, (int32_T)sizeof
                      (real_T));
    ib = ix - 1;
    for (ix = 0; ix <= ib; ix++) {
      b_previouspathq->data[ix] = previouspathq->data[ix + previouspathq->size[0]];
    }

    emxInit_boolean_T(&matchpaths, 1);
    emxInit_real_T(&c_previouspathq, 1);
    ismember(mans, b_previouspathq, matchpaths);

    /* 'equateconscost:12' matchpathsprev = ismember(previouspathq(:,2), mans) */
    ix = previouspathq->size[0];
    vlen = c_previouspathq->size[0];
    c_previouspathq->size[0] = ix;
    emxEnsureCapacity((emxArray__common *)c_previouspathq, vlen, (int32_T)sizeof
                      (real_T));
    emxFree_real_T(&b_previouspathq);
    ib = ix - 1;
    for (ix = 0; ix <= ib; ix++) {
      c_previouspathq->data[ix] = previouspathq->data[ix + previouspathq->size[0]];
    }

    emxInit_boolean_T(&matchpathsprev, 1);
    b_emxInit_real_T(&commonpathqi, 2);
    b_emxInit_int32_T(&r25, 1);
    ismember(c_previouspathq, mans, matchpathsprev);

    /* 'equateconscost:14' commonpathqi = pathqi(matchpaths,:); */
    eml_li_find(matchpaths, r25);
    ix = pathqi->size[1];
    vlen = commonpathqi->size[0] * commonpathqi->size[1];
    commonpathqi->size[0] = r25->size[0];
    commonpathqi->size[1] = ix;
    emxEnsureCapacity((emxArray__common *)commonpathqi, vlen, (int32_T)sizeof
                      (real_T));
    emxFree_real_T(&c_previouspathq);
    emxFree_boolean_T(&matchpaths);
    ib = ix - 1;
    for (ix = 0; ix <= ib; ix++) {
      ia = r25->size[0] - 1;
      for (vlen = 0; vlen <= ia; vlen++) {
        commonpathqi->data[vlen + commonpathqi->size[0] * ix] = pathqi->data
          [(r25->data[vlen] + pathqi->size[0] * ix) - 1];
      }
    }

    /* 'equateconscost:15' commonpreviousq = previouspathq(matchpathsprev,1); */
    eml_li_find(matchpathsprev, r25);
    ix = commonpreviousq->size[0];
    commonpreviousq->size[0] = r25->size[0];
    emxEnsureCapacity((emxArray__common *)commonpreviousq, ix, (int32_T)sizeof
                      (real_T));
    emxFree_boolean_T(&matchpathsprev);
    ib = r25->size[0] - 1;
    for (ix = 0; ix <= ib; ix++) {
      commonpreviousq->data[ix] = previouspathq->data[r25->data[ix] - 1];
    }

    emxFree_int32_T(&r25);

    /* 'equateconscost:16' [~, length] = size(commonpreviousq); */
    /* 'equateconscost:17' [~, npaths] = size(commonpathqi); */
    /* 'equateconscost:19' repmat(commonpreviousq, length, npaths); */
    /* 'equateconscost:21' distance = abs(commonpathqi - repmat(commonpreviousq, length, npaths)); */
    mv[0] = 1;
    mv[1] = commonpathqi->size[1];
    b_commonpreviousq[0] = commonpreviousq->size[0];
    b_commonpreviousq[1] = 1;
    for (ix = 0; ix < 2; ix++) {
      outsize[ix] = b_commonpreviousq[ix] * mv[ix];
    }

    b_emxInit_real_T(&b, 2);
    ix = b->size[0] * b->size[1];
    b->size[0] = outsize[0];
    b->size[1] = outsize[1];
    emxEnsureCapacity((emxArray__common *)b, ix, (int32_T)sizeof(real_T));
    if ((b->size[0] == 0) || (b->size[1] == 0)) {
    } else {
      ib = 0;
      for (vlen = 1; vlen <= mv[1]; vlen++) {
        ia = 0;
        for (k = 1; k <= commonpreviousq->size[0]; k++) {
          b->data[ib] = commonpreviousq->data[ia];
          ia++;
          ib++;
        }
      }
    }

    emxFree_real_T(&commonpreviousq);
    ix = commonpathqi->size[0] * commonpathqi->size[1];
    commonpathqi->size[0] = commonpathqi->size[0];
    commonpathqi->size[1] = commonpathqi->size[1];
    emxEnsureCapacity((emxArray__common *)commonpathqi, ix, (int32_T)sizeof
                      (real_T));
    ib = commonpathqi->size[0];
    vlen = commonpathqi->size[1];
    ib = ib * vlen - 1;
    for (ix = 0; ix <= ib; ix++) {
      commonpathqi->data[ix] -= b->data[ix];
    }

    for (ix = 0; ix < 2; ix++) {
      sz[ix] = (uint32_T)commonpathqi->size[ix];
    }

    ix = b->size[0] * b->size[1];
    b->size[0] = (int32_T)sz[0];
    b->size[1] = (int32_T)sz[1];
    emxEnsureCapacity((emxArray__common *)b, ix, (int32_T)sizeof(real_T));
    ix = commonpathqi->size[0] * commonpathqi->size[1];
    for (k = 0; k <= ix - 1; k++) {
      b->data[k] = fabs(commonpathqi->data[k]);
    }

    emxFree_real_T(&commonpathqi);

    /* 'equateconscost:23' total = sum(distance); */
    for (ix = 0; ix < 2; ix++) {
      sz[ix] = (uint32_T)b->size[ix];
    }

    sz[0] = 1U;
    ix = pathconscost->size[0] * pathconscost->size[1];
    pathconscost->size[0] = 1;
    pathconscost->size[1] = (int32_T)sz[1];
    emxEnsureCapacity((emxArray__common *)pathconscost, ix, (int32_T)sizeof
                      (real_T));
    if ((b->size[0] == 0) || (b->size[1] == 0)) {
      ix = pathconscost->size[0] * pathconscost->size[1];
      pathconscost->size[0] = 1;
      pathconscost->size[1] = pathconscost->size[1];
      emxEnsureCapacity((emxArray__common *)pathconscost, ix, (int32_T)sizeof
                        (real_T));
      ib = pathconscost->size[1] - 1;
      for (ix = 0; ix <= ib; ix++) {
        pathconscost->data[pathconscost->size[0] * ix] = 0.0;
      }
    } else {
      vlen = b->size[0];
      ia = b->size[1];
      ix = -1;
      iy = -1;
      for (i = 1; i <= ia; i++) {
        ib = ix + 1;
        ix++;
        s = b->data[ib];
        for (k = 2; k <= vlen; k++) {
          ix++;
          s += b->data[ix];
        }

        iy++;
        pathconscost->data[iy] = s;
      }
    }

    emxFree_real_T(&b);

    /* 'equateconscost:25' pathconscost = (1/(previouspathq(end,2) - mans(1))).*total; */
    s = previouspathq->data[(previouspathq->size[0] + previouspathq->size[0]) -
      1] - mans->data[0];
    s = 1.0 / s;
    ix = pathconscost->size[0] * pathconscost->size[1];
    pathconscost->size[0] = 1;
    pathconscost->size[1] = pathconscost->size[1];
    emxEnsureCapacity((emxArray__common *)pathconscost, ix, (int32_T)sizeof
                      (real_T));
    ib = pathconscost->size[0];
    vlen = pathconscost->size[1];
    ib = ib * vlen - 1;
    for (ix = 0; ix <= ib; ix++) {
      pathconscost->data[ix] *= s;
    }
  } else {
    /* 'equateconscost:26' else */
    /* 'equateconscost:27' [~,npaths] = size(pathqi); */
    /* 'equateconscost:28' pathconscost = zeros(1, npaths); */
    ix = pathconscost->size[0] * pathconscost->size[1];
    pathconscost->size[0] = 1;
    emxEnsureCapacity((emxArray__common *)pathconscost, ix, (int32_T)sizeof
                      (real_T));
    ib = pathqi->size[1];
    ix = pathconscost->size[0] * pathconscost->size[1];
    pathconscost->size[1] = ib;
    emxEnsureCapacity((emxArray__common *)pathconscost, ix, (int32_T)sizeof
                      (real_T));
    ib = pathqi->size[1] - 1;
    for (ix = 0; ix <= ib; ix++) {
      pathconscost->data[ix] = 0.0;
    }
  }
}

/* End of code generation (equateconscost.cpp) */
