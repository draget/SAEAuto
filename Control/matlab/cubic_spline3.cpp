/*
 * cubic_spline3.cpp
 *
 * Code generation for function 'cubic_spline3'
 *
 * C source code generated on: Wed Jul 23 14:27:34 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "builddetailedbf.h"
#include "cubic_spline3.h"
#include "matlab_emxutil.h"
#include "cubic_spline.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function [coefx, coefy] = cubic_spline3(x,y,t)
 */
void cubic_spline3(const emxArray_real_T *x, const emxArray_real_T *y, const
                   emxArray_real_T *t, emxArray_real_T *coefx, emxArray_real_T
                   *coefy)
{
  emxArray_real_T *x3;
  emxArray_real_T *x2;
  emxArray_real_T *x1;
  emxArray_real_T *x0;
  emxArray_real_T *y3;
  emxArray_real_T *y2;
  emxArray_real_T *b_y1;
  emxArray_real_T *b_y0;
  emxArray_real_T *b_x1;
  int32_T x0_idx_0;
  int32_T x1_idx_0;
  int32_T b_x1_idx_0;
  int32_T i1;
  int32_T loop_ub;
  emxArray_real_T *b_x2;
  int32_T x2_idx_0;
  emxArray_real_T *b_x3;
  int32_T x3_idx_0;
  emxArray_real_T *c_y1;
  emxArray_real_T *b_y2;
  emxArray_real_T *b_y3;
  emxInit_real_T(&x3, 1);
  emxInit_real_T(&x2, 1);
  emxInit_real_T(&x1, 1);
  emxInit_real_T(&x0, 1);
  emxInit_real_T(&y3, 1);
  emxInit_real_T(&y2, 1);
  emxInit_real_T(&b_y1, 1);
  emxInit_real_T(&b_y0, 1);
  b_emxInit_real_T(&b_x1, 2);

  /* 'cubic_spline3:2' [x3,x2,x1,x0] = cubic_spline(t,x); */
  cubic_spline(t, x, x3, x2, x1, x0);

  /* 'cubic_spline3:3' [y3,y2,y1,y0] = cubic_spline(t,y); */
  cubic_spline(t, y, y3, y2, b_y1, b_y0);

  /* 'cubic_spline3:5' coefx = [x0 x1 x2 x3]; */
  x0_idx_0 = x0->size[0];
  x1_idx_0 = x1->size[0];
  b_x1_idx_0 = x1->size[0];
  i1 = b_x1->size[0] * b_x1->size[1];
  b_x1->size[0] = x1_idx_0;
  b_x1->size[1] = 1;
  emxEnsureCapacity((emxArray__common *)b_x1, i1, (int32_T)sizeof(real_T));
  i1 = 0;
  while (i1 <= 0) {
    loop_ub = x1_idx_0 - 1;
    for (i1 = 0; i1 <= loop_ub; i1++) {
      b_x1->data[i1] = x1->data[i1];
    }

    i1 = 1;
  }

  emxFree_real_T(&x1);
  b_emxInit_real_T(&b_x2, 2);
  x1_idx_0 = x2->size[0];
  x2_idx_0 = x2->size[0];
  i1 = b_x2->size[0] * b_x2->size[1];
  b_x2->size[0] = x1_idx_0;
  b_x2->size[1] = 1;
  emxEnsureCapacity((emxArray__common *)b_x2, i1, (int32_T)sizeof(real_T));
  i1 = 0;
  while (i1 <= 0) {
    loop_ub = x1_idx_0 - 1;
    for (i1 = 0; i1 <= loop_ub; i1++) {
      b_x2->data[i1] = x2->data[i1];
    }

    i1 = 1;
  }

  emxFree_real_T(&x2);
  b_emxInit_real_T(&b_x3, 2);
  x1_idx_0 = x3->size[0];
  x3_idx_0 = x3->size[0];
  i1 = b_x3->size[0] * b_x3->size[1];
  b_x3->size[0] = x1_idx_0;
  b_x3->size[1] = 1;
  emxEnsureCapacity((emxArray__common *)b_x3, i1, (int32_T)sizeof(real_T));
  i1 = 0;
  while (i1 <= 0) {
    loop_ub = x1_idx_0 - 1;
    for (i1 = 0; i1 <= loop_ub; i1++) {
      b_x3->data[i1] = x3->data[i1];
    }

    i1 = 1;
  }

  emxFree_real_T(&x3);
  i1 = coefx->size[0] * coefx->size[1];
  coefx->size[0] = x0_idx_0;
  coefx->size[1] = 4;
  emxEnsureCapacity((emxArray__common *)coefx, i1, (int32_T)sizeof(real_T));
  i1 = 0;
  while (i1 <= 0) {
    loop_ub = x0_idx_0 - 1;
    for (i1 = 0; i1 <= loop_ub; i1++) {
      coefx->data[i1] = x0->data[i1];
    }

    i1 = 1;
  }

  emxFree_real_T(&x0);
  loop_ub = b_x1_idx_0 - 1;
  for (i1 = 0; i1 <= loop_ub; i1++) {
    coefx->data[i1 + coefx->size[0]] = b_x1->data[i1];
  }

  emxFree_real_T(&b_x1);
  loop_ub = x2_idx_0 - 1;
  for (i1 = 0; i1 <= loop_ub; i1++) {
    coefx->data[i1 + coefx->size[0] * 2] = b_x2->data[i1];
  }

  emxFree_real_T(&b_x2);
  loop_ub = x3_idx_0 - 1;
  for (i1 = 0; i1 <= loop_ub; i1++) {
    coefx->data[i1 + coefx->size[0] * 3] = b_x3->data[i1];
  }

  emxFree_real_T(&b_x3);
  b_emxInit_real_T(&c_y1, 2);

  /* 'cubic_spline3:6' coefy = [y0 y1 y2 y3]; */
  x0_idx_0 = b_y0->size[0];
  x1_idx_0 = b_y1->size[0];
  b_x1_idx_0 = b_y1->size[0];
  i1 = c_y1->size[0] * c_y1->size[1];
  c_y1->size[0] = x1_idx_0;
  c_y1->size[1] = 1;
  emxEnsureCapacity((emxArray__common *)c_y1, i1, (int32_T)sizeof(real_T));
  i1 = 0;
  while (i1 <= 0) {
    loop_ub = x1_idx_0 - 1;
    for (i1 = 0; i1 <= loop_ub; i1++) {
      c_y1->data[i1] = b_y1->data[i1];
    }

    i1 = 1;
  }

  emxFree_real_T(&b_y1);
  b_emxInit_real_T(&b_y2, 2);
  x1_idx_0 = y2->size[0];
  x2_idx_0 = y2->size[0];
  i1 = b_y2->size[0] * b_y2->size[1];
  b_y2->size[0] = x1_idx_0;
  b_y2->size[1] = 1;
  emxEnsureCapacity((emxArray__common *)b_y2, i1, (int32_T)sizeof(real_T));
  i1 = 0;
  while (i1 <= 0) {
    loop_ub = x1_idx_0 - 1;
    for (i1 = 0; i1 <= loop_ub; i1++) {
      b_y2->data[i1] = y2->data[i1];
    }

    i1 = 1;
  }

  emxFree_real_T(&y2);
  b_emxInit_real_T(&b_y3, 2);
  x1_idx_0 = y3->size[0];
  x3_idx_0 = y3->size[0];
  i1 = b_y3->size[0] * b_y3->size[1];
  b_y3->size[0] = x1_idx_0;
  b_y3->size[1] = 1;
  emxEnsureCapacity((emxArray__common *)b_y3, i1, (int32_T)sizeof(real_T));
  i1 = 0;
  while (i1 <= 0) {
    loop_ub = x1_idx_0 - 1;
    for (i1 = 0; i1 <= loop_ub; i1++) {
      b_y3->data[i1] = y3->data[i1];
    }

    i1 = 1;
  }

  emxFree_real_T(&y3);
  i1 = coefy->size[0] * coefy->size[1];
  coefy->size[0] = x0_idx_0;
  coefy->size[1] = 4;
  emxEnsureCapacity((emxArray__common *)coefy, i1, (int32_T)sizeof(real_T));
  i1 = 0;
  while (i1 <= 0) {
    loop_ub = x0_idx_0 - 1;
    for (i1 = 0; i1 <= loop_ub; i1++) {
      coefy->data[i1] = b_y0->data[i1];
    }

    i1 = 1;
  }

  emxFree_real_T(&b_y0);
  loop_ub = b_x1_idx_0 - 1;
  for (i1 = 0; i1 <= loop_ub; i1++) {
    coefy->data[i1 + coefy->size[0]] = c_y1->data[i1];
  }

  emxFree_real_T(&c_y1);
  loop_ub = x2_idx_0 - 1;
  for (i1 = 0; i1 <= loop_ub; i1++) {
    coefy->data[i1 + coefy->size[0] * 2] = b_y2->data[i1];
  }

  emxFree_real_T(&b_y2);
  loop_ub = x3_idx_0 - 1;
  for (i1 = 0; i1 <= loop_ub; i1++) {
    coefy->data[i1 + coefy->size[0] * 3] = b_y3->data[i1];
  }

  emxFree_real_T(&b_y3);
}

/* End of code generation (cubic_spline3.cpp) */
