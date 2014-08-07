/*
 * cubic_spline.cpp
 *
 * Code generation for function 'cubic_spline'
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
#include "cubic_spline.h"
#include "matlab_emxutil.h"
#include "rdivide.h"
#include "mldivide.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function [s0,s1,s2,s3] = cubic_spline(x,y)
 */
void cubic_spline(const emxArray_real_T *x, const emxArray_real_T *y,
                  emxArray_real_T *s0, emxArray_real_T *s1, emxArray_real_T *s2,
                  emxArray_real_T *s3)
{
  int32_T i2;
  int32_T nv;
  emxArray_real_T *h;
  int32_T m;
  emxArray_real_T *b_y;
  emxArray_real_T *d;
  emxArray_int32_T *r0;
  emxArray_int32_T *r1;
  emxArray_real_T *main;
  emxArray_int32_T *r2;
  emxArray_int32_T *r3;
  emxArray_real_T *b_h;
  emxArray_real_T *r4;
  emxArray_real_T *r5;
  emxArray_real_T *c_h;
  emxArray_real_T *r6;
  emxArray_real_T *r7;
  emxArray_real_T *r8;
  emxArray_real_T *b_main;
  emxArray_real_T *r9;

  /* 'cubic_spline:3' if any(size(x) ~= size(y)) || size(x,2) ~= 1 */
  /* 'cubic_spline:7' n = length(x); */
  /* 'cubic_spline:9' h = x(2:n) - x(1:n-1); */
  if (2 > x->size[0]) {
    i2 = 0;
    nv = 0;
  } else {
    i2 = 1;
    nv = x->size[0];
  }

  emxInit_real_T(&h, 1);
  m = h->size[0];
  h->size[0] = nv - i2;
  emxEnsureCapacity((emxArray__common *)h, m, (int32_T)sizeof(real_T));
  m = (nv - i2) - 1;
  for (nv = 0; nv <= m; nv++) {
    h->data[nv] = x->data[i2 + nv] - x->data[nv];
  }

  /* 'cubic_spline:10' d = (y(2:n) - y(1:n-1))./h; */
  if (2 > x->size[0]) {
    i2 = 0;
    nv = 0;
  } else {
    i2 = 1;
    nv = x->size[0];
  }

  emxInit_real_T(&b_y, 1);
  m = b_y->size[0];
  b_y->size[0] = nv - i2;
  emxEnsureCapacity((emxArray__common *)b_y, m, (int32_T)sizeof(real_T));
  m = (nv - i2) - 1;
  for (nv = 0; nv <= m; nv++) {
    b_y->data[nv] = y->data[i2 + nv] - y->data[nv];
  }

  emxInit_real_T(&d, 1);
  rdivide(b_y, h, d);

  /* 'cubic_spline:12' lower = h(2:end-1); */
  emxFree_real_T(&b_y);
  if (2 > h->size[0] - 1) {
    i2 = 1;
    nv = 1;
  } else {
    i2 = 2;
    nv = h->size[0];
  }

  b_emxInit_int32_T(&r0, 1);
  m = r0->size[0];
  r0->size[0] = nv - i2;
  emxEnsureCapacity((emxArray__common *)r0, m, (int32_T)sizeof(int32_T));
  m = (nv - i2) - 1;
  for (nv = 0; nv <= m; nv++) {
    r0->data[nv] = i2 + nv;
  }

  emxInit_int32_T(&r1, 2);
  i2 = r1->size[0] * r1->size[1];
  r1->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)r1, i2, (int32_T)sizeof(int32_T));
  m = r0->size[0];
  i2 = r1->size[0] * r1->size[1];
  r1->size[1] = m;
  emxEnsureCapacity((emxArray__common *)r1, i2, (int32_T)sizeof(int32_T));
  m = r0->size[0] - 1;
  for (i2 = 0; i2 <= m; i2++) {
    r1->data[i2] = r0->data[i2];
  }

  emxFree_int32_T(&r0);

  /* 'cubic_spline:13' main  = 2*(h(1:end-1) + h(2:end)); */
  if (1 > h->size[0] - 1) {
    i2 = -1;
  } else {
    i2 = h->size[0] - 2;
  }

  if (2 > h->size[0]) {
    nv = 0;
  } else {
    nv = 1;
  }

  emxInit_real_T(&main, 1);
  m = main->size[0];
  main->size[0] = i2 + 1;
  emxEnsureCapacity((emxArray__common *)main, m, (int32_T)sizeof(real_T));
  for (m = 0; m <= i2; m++) {
    main->data[m] = 2.0 * (h->data[m] + h->data[nv + m]);
  }

  /* 'cubic_spline:14' upper = h(1:end-2); */
  if (1 > h->size[0] - 2) {
    i2 = -1;
  } else {
    i2 = h->size[0] - 3;
  }

  b_emxInit_int32_T(&r2, 1);
  nv = r2->size[0];
  r2->size[0] = i2 + 1;
  emxEnsureCapacity((emxArray__common *)r2, nv, (int32_T)sizeof(int32_T));
  for (nv = 0; nv <= i2; nv++) {
    r2->data[nv] = 1 + nv;
  }

  emxInit_int32_T(&r3, 2);
  i2 = r3->size[0] * r3->size[1];
  r3->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)r3, i2, (int32_T)sizeof(int32_T));
  m = r2->size[0];
  i2 = r3->size[0] * r3->size[1];
  r3->size[1] = m;
  emxEnsureCapacity((emxArray__common *)r3, i2, (int32_T)sizeof(int32_T));
  m = r2->size[0] - 1;
  for (i2 = 0; i2 <= m; i2++) {
    r3->data[i2] = r2->data[i2];
  }

  emxFree_int32_T(&r2);
  emxInit_real_T(&b_h, 1);

  /* 'cubic_spline:16' T = diag(lower,-1) + diag(main) + diag(upper,1); */
  m = r1->size[1];
  i2 = b_h->size[0];
  b_h->size[0] = m;
  emxEnsureCapacity((emxArray__common *)b_h, i2, (int32_T)sizeof(real_T));
  m--;
  for (i2 = 0; i2 <= m; i2++) {
    b_h->data[i2] = h->data[r1->data[i2] - 1];
  }

  b_emxInit_real_T(&r4, 2);
  nv = b_h->size[0];
  i2 = r4->size[0] * r4->size[1];
  r4->size[0] = nv + 1;
  emxEnsureCapacity((emxArray__common *)r4, i2, (int32_T)sizeof(real_T));
  i2 = r4->size[0] * r4->size[1];
  r4->size[1] = nv + 1;
  emxEnsureCapacity((emxArray__common *)r4, i2, (int32_T)sizeof(real_T));
  emxFree_real_T(&b_h);
  m = (nv + 1) * (nv + 1) - 1;
  for (i2 = 0; i2 <= m; i2++) {
    r4->data[i2] = 0.0;
  }

  for (m = 1; m <= nv; m++) {
    r4->data[m + r4->size[0] * (m - 1)] = h->data[r1->data[m - 1] - 1];
  }

  emxFree_int32_T(&r1);
  b_emxInit_real_T(&r5, 2);
  nv = main->size[0];
  i2 = r5->size[0] * r5->size[1];
  r5->size[0] = nv;
  emxEnsureCapacity((emxArray__common *)r5, i2, (int32_T)sizeof(real_T));
  i2 = r5->size[0] * r5->size[1];
  r5->size[1] = nv;
  emxEnsureCapacity((emxArray__common *)r5, i2, (int32_T)sizeof(real_T));
  m = nv * nv - 1;
  for (i2 = 0; i2 <= m; i2++) {
    r5->data[i2] = 0.0;
  }

  for (m = 0; m + 1 <= nv; m++) {
    r5->data[m + r5->size[0] * m] = main->data[m];
  }

  emxInit_real_T(&c_h, 1);
  m = r3->size[1];
  i2 = c_h->size[0];
  c_h->size[0] = m;
  emxEnsureCapacity((emxArray__common *)c_h, i2, (int32_T)sizeof(real_T));
  m--;
  for (i2 = 0; i2 <= m; i2++) {
    c_h->data[i2] = h->data[r3->data[i2] - 1];
  }

  b_emxInit_real_T(&r6, 2);
  nv = c_h->size[0];
  m = nv + 1;
  i2 = r6->size[0] * r6->size[1];
  r6->size[0] = m;
  emxEnsureCapacity((emxArray__common *)r6, i2, (int32_T)sizeof(real_T));
  i2 = r6->size[0] * r6->size[1];
  r6->size[1] = m;
  emxEnsureCapacity((emxArray__common *)r6, i2, (int32_T)sizeof(real_T));
  emxFree_real_T(&c_h);
  m = m * m - 1;
  for (i2 = 0; i2 <= m; i2++) {
    r6->data[i2] = 0.0;
  }

  for (m = 1; m <= nv; m++) {
    r6->data[(m + r6->size[0] * m) - 1] = h->data[r3->data[m - 1] - 1];
  }

  emxFree_int32_T(&r3);

  /* 'cubic_spline:18' rhs = 6*(d(2:end)-d(1:end-1)); */
  if (2 > d->size[0]) {
    i2 = 0;
    nv = 0;
  } else {
    i2 = 1;
    nv = d->size[0];
  }

  /* 'cubic_spline:20' m = T\rhs; */
  m = main->size[0];
  main->size[0] = nv - i2;
  emxEnsureCapacity((emxArray__common *)main, m, (int32_T)sizeof(real_T));
  m = (nv - i2) - 1;
  for (nv = 0; nv <= m; nv++) {
    main->data[nv] = 6.0 * (d->data[i2 + nv] - d->data[nv]);
  }

  b_emxInit_real_T(&r7, 2);
  i2 = r7->size[0] * r7->size[1];
  r7->size[0] = r4->size[0];
  r7->size[1] = r4->size[1];
  emxEnsureCapacity((emxArray__common *)r7, i2, (int32_T)sizeof(real_T));
  m = r4->size[0] * r4->size[1] - 1;
  for (i2 = 0; i2 <= m; i2++) {
    r7->data[i2] = (r4->data[i2] + r5->data[i2]) + r6->data[i2];
  }

  emxFree_real_T(&r6);
  emxFree_real_T(&r5);
  emxFree_real_T(&r4);
  emxInit_real_T(&r8, 1);
  mldivide(r7, main);

  /*  Use natural boundary conditions where second derivative */
  /*  is zero at the endpoints */
  /* 'cubic_spline:25' m = [ 0; m; 0]; */
  i2 = r8->size[0];
  r8->size[0] = 2 + main->size[0];
  emxEnsureCapacity((emxArray__common *)r8, i2, (int32_T)sizeof(real_T));
  r8->data[0] = 0.0;
  emxFree_real_T(&r7);
  m = main->size[0] - 1;
  for (i2 = 0; i2 <= m; i2++) {
    r8->data[i2 + 1] = main->data[i2];
  }

  r8->data[1 + main->size[0]] = 0.0;
  i2 = main->size[0];
  main->size[0] = r8->size[0];
  emxEnsureCapacity((emxArray__common *)main, i2, (int32_T)sizeof(real_T));
  m = r8->size[0] - 1;
  for (i2 = 0; i2 <= m; i2++) {
    main->data[i2] = r8->data[i2];
  }

  emxFree_real_T(&r8);

  /* 'cubic_spline:27' s0 = y(1:end-1); */
  if (1 > y->size[0] - 1) {
    i2 = -1;
  } else {
    i2 = y->size[0] - 2;
  }

  nv = s0->size[0];
  s0->size[0] = i2 + 1;
  emxEnsureCapacity((emxArray__common *)s0, nv, (int32_T)sizeof(real_T));
  for (nv = 0; nv <= i2; nv++) {
    s0->data[nv] = y->data[nv];
  }

  /* 'cubic_spline:28' s1 = d - h.*(2*m(1:end-1) + m(2:end))/6; */
  i2 = s1->size[0];
  s1->size[0] = d->size[0];
  emxEnsureCapacity((emxArray__common *)s1, i2, (int32_T)sizeof(real_T));
  m = d->size[0] - 1;
  for (i2 = 0; i2 <= m; i2++) {
    s1->data[i2] = d->data[i2] - h->data[i2] * (2.0 * main->data[i2] +
      main->data[1 + i2]) / 6.0;
  }

  emxFree_real_T(&d);

  /* 'cubic_spline:29' s2 = m(1:end-1)/2; */
  i2 = main->size[0] - 2;
  nv = s2->size[0];
  s2->size[0] = i2 + 1;
  emxEnsureCapacity((emxArray__common *)s2, nv, (int32_T)sizeof(real_T));
  for (nv = 0; nv <= i2; nv++) {
    s2->data[nv] = main->data[nv] / 2.0;
  }

  emxInit_real_T(&b_main, 1);

  /* 'cubic_spline:30' s3 =(m(2:end)-m(1:end-1))./(6*h); */
  i2 = main->size[0] - 2;
  nv = b_main->size[0];
  b_main->size[0] = i2 + 1;
  emxEnsureCapacity((emxArray__common *)b_main, nv, (int32_T)sizeof(real_T));
  for (nv = 0; nv <= i2; nv++) {
    b_main->data[nv] = main->data[1 + nv] - main->data[nv];
  }

  emxFree_real_T(&main);
  emxInit_real_T(&r9, 1);
  i2 = r9->size[0];
  r9->size[0] = h->size[0];
  emxEnsureCapacity((emxArray__common *)r9, i2, (int32_T)sizeof(real_T));
  m = h->size[0] - 1;
  for (i2 = 0; i2 <= m; i2++) {
    r9->data[i2] = 6.0 * h->data[i2];
  }

  emxFree_real_T(&h);
  rdivide(b_main, r9, s3);
  emxFree_real_T(&r9);
  emxFree_real_T(&b_main);
}

/* End of code generation (cubic_spline.cpp) */
