/*
 * mldivide.cpp
 *
 * Code generation for function 'mldivide'
 *
 * C source code generated on: Wed Jul 23 22:33:34 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "builddetailedbf.h"
#include "mldivide.h"
#include "matlab_emxutil.h"
#include "colon.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
static real_T eml_div(real_T x, real_T y);
static void eml_matlab_zlarf(int32_T m, int32_T n, int32_T iv0, real_T tau,
  emxArray_real_T *C, int32_T ic0, int32_T ldc, emxArray_real_T *work);
static void eml_qrsolve(const emxArray_real_T *A, emxArray_real_T *B,
  emxArray_real_T *Y);
static real_T eml_xnrm2(int32_T n, const emxArray_real_T *x, int32_T ix0);
static void eml_xscal(int32_T n, real_T a, emxArray_real_T *x, int32_T ix0);
static void eml_xswap(int32_T n, emxArray_real_T *x, int32_T ix0, int32_T incx,
                      int32_T iy0, int32_T incy);
static real_T rt_hypotd_snf(real_T u0, real_T u1);

/* Function Definitions */

/*
 *
 */
static real_T eml_div(real_T x, real_T y)
{
  return x / y;
}

/*
 *
 */
static void eml_matlab_zlarf(int32_T m, int32_T n, int32_T iv0, real_T tau,
  emxArray_real_T *C, int32_T ic0, int32_T ldc, emxArray_real_T *work)
{
  emxArray_real_T *b_C;
  emxArray_real_T *b_work;
  int32_T lastv;
  int32_T i;
  int32_T lastc;
  boolean_T exitg2;
  int32_T ia;
  int32_T exitg1;
  int32_T iy;
  int32_T i7;
  int32_T ix;
  real_T c;
  int32_T jy;
  b_emxInit_real_T(&b_C, 2);
  emxInit_real_T(&b_work, 1);
  if (tau != 0.0) {
    lastv = m;
    i = iv0 + m;
    while ((lastv > 0) && (C->data[i - 2] == 0.0)) {
      lastv--;
      i--;
    }

    lastc = n;
    exitg2 = FALSE;
    while ((exitg2 == 0U) && (lastc > 0)) {
      i = ic0 + (lastc - 1) * ldc;
      ia = i;
      do {
        exitg1 = 0;
        if (ia <= (i + lastv) - 1) {
          if (C->data[ia - 1] != 0.0) {
            exitg1 = 1;
          } else {
            ia++;
          }
        } else {
          lastc--;
          exitg1 = 2;
        }
      } while (exitg1 == 0U);

      if (exitg1 == 1U) {
        exitg2 = TRUE;
      }
    }
  } else {
    lastv = 0;
    lastc = 0;
  }

  if (lastv > 0) {
    if (lastc == 0) {
    } else {
      i = lastc - 1;
      for (iy = 1; iy <= i + 1; iy++) {
        work->data[iy - 1] = 0.0;
      }

      iy = 0;
      i7 = ic0 + ldc * i;
      for (i = ic0; i <= i7; i += ldc) {
        ix = iv0;
        c = 0.0;
        jy = i + lastv;
        for (ia = i; ia <= jy - 1; ia++) {
          c += C->data[ia - 1] * C->data[ix - 1];
          ix++;
        }

        work->data[iy] += c;
        iy++;
      }
    }

    if (-tau == 0.0) {
    } else {
      i = ic0 - 1;
      jy = 0;
      for (ia = 1; ia <= lastc; ia++) {
        if (work->data[jy] != 0.0) {
          c = work->data[jy] * -tau;
          ix = iv0;
          i7 = lastv + i;
          for (iy = i; iy + 1 <= i7; iy++) {
            C->data[iy] += C->data[ix - 1] * c;
            ix++;
          }
        }

        jy++;
        i += ldc;
      }
    }
  }

  emxFree_real_T(&b_work);
  emxFree_real_T(&b_C);
}

/*
 *
 */
static void eml_qrsolve(const emxArray_real_T *A, emxArray_real_T *B,
  emxArray_real_T *Y)
{
  emxArray_real_T *b_A;
  real_T wj;
  real_T s;
  int32_T mn;
  int32_T nmip1;
  int32_T itemp;
  emxArray_real_T *tau;
  emxArray_int32_T *jpvt;
  emxArray_real_T *work;
  int32_T m;
  int32_T n;
  int32_T b_mn;
  emxArray_real_T *vn1;
  emxArray_real_T *vn2;
  int32_T k;
  int32_T ix;
  int32_T i;
  int32_T i_i;
  int32_T nmi;
  int32_T mmi;
  real_T rankR;
  real_T y;
  boolean_T exitg1;
  uint32_T unnamed_idx_0;
  b_emxInit_real_T(&b_A, 2);
  wj = (real_T)A->size[0];
  s = (real_T)A->size[1];
  if (wj <= s) {
    s = wj;
  }

  mn = (int32_T)s - 1;
  nmip1 = b_A->size[0] * b_A->size[1];
  b_A->size[0] = A->size[0];
  b_A->size[1] = A->size[1];
  emxEnsureCapacity((emxArray__common *)b_A, nmip1, (int32_T)sizeof(real_T));
  itemp = A->size[0] * A->size[1] - 1;
  for (nmip1 = 0; nmip1 <= itemp; nmip1++) {
    b_A->data[nmip1] = A->data[nmip1];
  }

  emxInit_real_T(&tau, 1);
  emxInit_int32_T(&jpvt, 2);
  emxInit_real_T(&work, 1);
  m = A->size[0];
  n = A->size[1];
  if (m <= n) {
    b_mn = m;
  } else {
    b_mn = n;
  }

  nmip1 = tau->size[0];
  tau->size[0] = b_mn;
  emxEnsureCapacity((emxArray__common *)tau, nmip1, (int32_T)sizeof(real_T));
  eml_signed_integer_colon(n, jpvt);
  nmip1 = work->size[0];
  work->size[0] = n;
  emxEnsureCapacity((emxArray__common *)work, nmip1, (int32_T)sizeof(real_T));
  itemp = n - 1;
  for (nmip1 = 0; nmip1 <= itemp; nmip1++) {
    work->data[nmip1] = 0.0;
  }

  emxInit_real_T(&vn1, 1);
  emxInit_real_T(&vn2, 1);
  nmip1 = vn1->size[0];
  vn1->size[0] = n;
  emxEnsureCapacity((emxArray__common *)vn1, nmip1, (int32_T)sizeof(real_T));
  nmip1 = vn2->size[0];
  vn2->size[0] = vn1->size[0];
  emxEnsureCapacity((emxArray__common *)vn2, nmip1, (int32_T)sizeof(real_T));
  k = 1;
  for (ix = 0; ix + 1 <= n; ix++) {
    vn1->data[ix] = eml_xnrm2(m, A, k);
    vn2->data[ix] = vn1->data[ix];
    k += m;
  }

  for (i = 0; i + 1 <= b_mn; i++) {
    i_i = i + i * m;
    nmi = (n - i) - 1;
    mmi = (m - i) - 1;
    nmip1 = 1 + nmi;
    if (nmip1 < 1) {
      itemp = -1;
    } else {
      itemp = 0;
      if (nmip1 > 1) {
        ix = i;
        wj = fabs(vn1->data[i]);
        for (k = 2; k <= nmip1; k++) {
          ix++;
          s = fabs(vn1->data[ix]);
          if (s > wj) {
            itemp = k - 1;
            wj = s;
          }
        }
      }
    }

    nmip1 = i + itemp;
    if (nmip1 + 1 != i + 1) {
      eml_xswap(m, b_A, m * nmip1 + 1, 1, m * i + 1, 1);
      itemp = jpvt->data[nmip1];
      jpvt->data[nmip1] = jpvt->data[i];
      jpvt->data[i] = itemp;
      vn1->data[nmip1] = vn1->data[i];
      vn2->data[nmip1] = vn2->data[i];
    }

    if (i + 1 < m) {
      k = i_i + 2;
      rankR = b_A->data[i_i];
      y = 0.0;
      if (mmi + 1 <= 0) {
      } else {
        wj = eml_xnrm2(mmi, b_A, k);
        if (wj != 0.0) {
          s = rt_hypotd_snf(fabs(b_A->data[i_i]), wj);
          if (b_A->data[i_i] >= 0.0) {
            s = -s;
          }

          if (fabs(s) < 1.0020841800044864E-292) {
            nmip1 = 0;
            do {
              nmip1++;
              eml_xscal(mmi, 9.9792015476736E+291, b_A, k);
              s *= 9.9792015476736E+291;
              rankR *= 9.9792015476736E+291;
            } while (!(fabs(s) >= 1.0020841800044864E-292));

            wj = eml_xnrm2(mmi, b_A, k);
            s = rt_hypotd_snf(fabs(rankR), wj);
            if (rankR >= 0.0) {
              s = -s;
            }

            y = (s - rankR) / s;
            eml_xscal(mmi, 1.0 / (rankR - s), b_A, k);
            for (k = 1; k <= nmip1; k++) {
              s *= 1.0020841800044864E-292;
            }

            rankR = s;
          } else {
            y = (s - b_A->data[i_i]) / s;
            wj = 1.0 / (b_A->data[i_i] - s);
            eml_xscal(mmi, wj, b_A, k);
            rankR = s;
          }
        }
      }

      tau->data[i] = y;
    } else {
      wj = b_A->data[i_i];
      rankR = b_A->data[i_i];
      b_A->data[i_i] = wj;
      tau->data[i] = 0.0;
    }

    b_A->data[i_i] = rankR;
    if (i + 1 < n) {
      rankR = b_A->data[i_i];
      b_A->data[i_i] = 1.0;
      eml_matlab_zlarf(mmi + 1, nmi, i_i + 1, tau->data[i], b_A, (i + (i + 1) *
        m) + 1, m, work);
      b_A->data[i_i] = rankR;
    }

    for (ix = i + 1; ix + 1 <= n; ix++) {
      if (vn1->data[ix] != 0.0) {
        s = fabs(b_A->data[i + b_A->size[0] * ix]) / vn1->data[ix];
        y = s * s;
        s = 1.0 - s * s;
        if (1.0 - y < 0.0) {
          s = 0.0;
        }

        wj = vn1->data[ix] / vn2->data[ix];
        if (s * (wj * wj) <= 1.4901161193847656E-8) {
          if (i + 1 < m) {
            k = (i + m * ix) + 1;
            y = 0.0;
            if (mmi < 1) {
            } else if (mmi == 1) {
              y = fabs(b_A->data[k]);
            } else {
              wj = 2.2250738585072014E-308;
              itemp = k + mmi;
              while (k + 1 <= itemp) {
                s = fabs(b_A->data[k]);
                if (s > wj) {
                  rankR = wj / s;
                  y = 1.0 + y * rankR * rankR;
                  wj = s;
                } else {
                  rankR = s / wj;
                  y += rankR * rankR;
                }

                k++;
              }

              y = wj * sqrt(y);
            }

            vn1->data[ix] = y;
            vn2->data[ix] = vn1->data[ix];
          } else {
            vn1->data[ix] = 0.0;
            vn2->data[ix] = 0.0;
          }
        } else {
          vn1->data[ix] *= sqrt(s);
        }
      }
    }
  }

  emxFree_real_T(&vn2);
  emxFree_real_T(&vn1);
  emxFree_real_T(&work);
  rankR = 0.0;
  k = 0;
  exitg1 = FALSE;
  while ((exitg1 == 0U) && (k <= mn)) {
    wj = (real_T)A->size[0];
    s = (real_T)A->size[1];
    if (wj >= s) {
      s = wj;
    }

    if (fabs(b_A->data[k + b_A->size[0] * k]) <= s * fabs(b_A->data[0]) *
        2.2204460492503131E-16) {
      exitg1 = TRUE;
    } else {
      rankR++;
      k++;
    }
  }

  unnamed_idx_0 = (uint32_T)A->size[1];
  nmip1 = Y->size[0];
  Y->size[0] = (int32_T)unnamed_idx_0;
  emxEnsureCapacity((emxArray__common *)Y, nmip1, (int32_T)sizeof(real_T));
  itemp = (int32_T)unnamed_idx_0 - 1;
  for (nmip1 = 0; nmip1 <= itemp; nmip1++) {
    Y->data[nmip1] = 0.0;
  }

  for (ix = 0; ix <= mn; ix++) {
    if (tau->data[ix] != 0.0) {
      wj = B->data[ix];
      nmip1 = A->size[0] + (int32_T)(1.0 - ((1.0 + (real_T)ix) + 1.0));
      for (i = 0; i <= nmip1 - 1; i++) {
        unnamed_idx_0 = ((uint32_T)ix + (uint32_T)i) + 2U;
        wj += b_A->data[((int32_T)unnamed_idx_0 + b_A->size[0] * ix) - 1] *
          B->data[(int32_T)unnamed_idx_0 - 1];
      }

      wj *= tau->data[ix];
      if (wj != 0.0) {
        B->data[ix] -= wj;
        nmip1 = A->size[0] + (int32_T)(1.0 - ((1.0 + (real_T)ix) + 1.0));
        for (i = 0; i <= nmip1 - 1; i++) {
          unnamed_idx_0 = ((uint32_T)ix + (uint32_T)i) + 2U;
          B->data[(int32_T)unnamed_idx_0 - 1] -= b_A->data[((int32_T)
            unnamed_idx_0 + b_A->size[0] * ix) - 1] * wj;
        }
      }
    }
  }

  emxFree_real_T(&tau);
  for (i = 0; i <= (int32_T)rankR - 1; i++) {
    Y->data[jpvt->data[(int32_T)(1.0 + (real_T)i) - 1] - 1] = B->data[(int32_T)
      (1.0 + (real_T)i) - 1];
  }

  for (ix = 0; ix <= (int32_T)-(1.0 + (-1.0 - rankR)) - 1; ix++) {
    wj = rankR + -(real_T)ix;
    Y->data[jpvt->data[(int32_T)wj - 1] - 1] = eml_div(Y->data[jpvt->data
      [(int32_T)wj - 1] - 1], b_A->data[((int32_T)wj + b_A->size[0] * ((int32_T)
      wj - 1)) - 1]);
    for (i = 0; i <= (int32_T)wj - 2; i++) {
      Y->data[jpvt->data[(int32_T)(1.0 + (real_T)i) - 1] - 1] -= Y->data
        [jpvt->data[(int32_T)wj - 1] - 1] * b_A->data[((int32_T)(1.0 + (real_T)i)
        + b_A->size[0] * ((int32_T)wj - 1)) - 1];
    }
  }

  emxFree_int32_T(&jpvt);
  emxFree_real_T(&b_A);
}

/*
 *
 */
static real_T eml_xnrm2(int32_T n, const emxArray_real_T *x, int32_T ix0)
{
  real_T y;
  real_T scale;
  int32_T kend;
  int32_T k;
  real_T absxk;
  real_T t;
  y = 0.0;
  if (n < 1) {
  } else if (n == 1) {
    y = fabs(x->data[ix0 - 1]);
  } else {
    scale = 2.2250738585072014E-308;
    kend = (ix0 + n) - 1;
    for (k = ix0; k <= kend; k++) {
      absxk = fabs(x->data[k - 1]);
      if (absxk > scale) {
        t = scale / absxk;
        y = 1.0 + y * t * t;
        scale = absxk;
      } else {
        t = absxk / scale;
        y += t * t;
      }
    }

    y = scale * sqrt(y);
  }

  return y;
}

/*
 *
 */
static void eml_xscal(int32_T n, real_T a, emxArray_real_T *x, int32_T ix0)
{
  emxArray_real_T *b_x;
  int32_T i6;
  int32_T k;
  b_emxInit_real_T(&b_x, 2);
  i6 = (ix0 + n) - 1;
  for (k = ix0; k <= i6; k++) {
    x->data[k - 1] *= a;
  }

  emxFree_real_T(&b_x);
}

/*
 *
 */
static void eml_xswap(int32_T n, emxArray_real_T *x, int32_T ix0, int32_T incx,
                      int32_T iy0, int32_T incy)
{
  emxArray_real_T *b_x;
  int32_T ix;
  int32_T iy;
  int32_T k;
  real_T temp;
  b_emxInit_real_T(&b_x, 2);
  ix = ix0 - 1;
  iy = iy0 - 1;
  for (k = 1; k <= n; k++) {
    temp = x->data[ix];
    x->data[ix] = x->data[iy];
    x->data[iy] = temp;
    ix += incx;
    iy += incy;
  }

  emxFree_real_T(&b_x);
}

static real_T rt_hypotd_snf(real_T u0, real_T u1)
{
  real_T y;
  real_T a;
  a = fabs(u0);
  y = fabs(u1);
  if (a < y) {
    a /= y;
    y *= sqrt(a * a + 1.0);
  } else if (a > y) {
    y /= a;
    y = a * sqrt(y * y + 1.0);
  } else if (rtIsNaN(y)) {
  } else {
    y = a * 1.4142135623730951;
  }

  return y;
}

/*
 *
 */
void mldivide(const emxArray_real_T *A, emxArray_real_T *B)
{
  emxArray_real_T *Y;
  emxArray_real_T *b_B;
  int32_T n;
  int32_T i5;
  int32_T jy;
  emxArray_int32_T *ipiv;
  int32_T ldap1;
  int32_T u0;
  int32_T j;
  int32_T mmj;
  int32_T jj;
  int32_T jp1j;
  int32_T c;
  int32_T ix;
  real_T temp;
  int32_T k;
  real_T s;
  int32_T jA;
  emxArray_real_T *r11;
  emxInit_real_T(&Y, 1);
  if (A->size[0] == A->size[1]) {
    b_emxInit_real_T(&b_B, 2);
    n = A->size[1];
    i5 = b_B->size[0] * b_B->size[1];
    b_B->size[0] = A->size[0];
    b_B->size[1] = A->size[1];
    emxEnsureCapacity((emxArray__common *)b_B, i5, (int32_T)sizeof(real_T));
    jy = A->size[0] * A->size[1] - 1;
    for (i5 = 0; i5 <= jy; i5++) {
      b_B->data[i5] = A->data[i5];
    }

    emxInit_int32_T(&ipiv, 2);
    eml_signed_integer_colon(n, ipiv);
    ldap1 = n + 1;
    u0 = n - 1;
    if (u0 <= n) {
    } else {
      u0 = n;
    }

    for (j = 1; j <= u0; j++) {
      mmj = n - j;
      jj = (j - 1) * ldap1;
      jp1j = jj + 2;
      c = mmj + 1;
      if (c < 1) {
        jy = 0;
      } else {
        jy = 1;
        if (c > 1) {
          ix = jj;
          temp = fabs(b_B->data[jj]);
          for (k = 2; k <= c; k++) {
            ix++;
            s = fabs(b_B->data[ix]);
            if (s > temp) {
              jy = k;
              temp = s;
            }
          }
        }
      }

      if (b_B->data[(jj + jy) - 1] != 0.0) {
        if (jy - 1 != 0) {
          ipiv->data[j - 1] = (j + jy) - 1;
          eml_xswap(n, b_B, j, n, (j + jy) - 1, n);
        }

        i5 = (jp1j + mmj) - 1;
        for (jy = jp1j; jy <= i5; jy++) {
          b_B->data[jy - 1] /= b_B->data[jj];
        }
      }

      c = n - j;
      jA = jj + ldap1;
      jy = jj + n;
      for (k = 1; k <= c; k++) {
        temp = b_B->data[jy];
        if (b_B->data[jy] != 0.0) {
          ix = jp1j;
          i5 = mmj + jA;
          for (jj = jA; jj + 1 <= i5; jj++) {
            b_B->data[jj] += b_B->data[ix - 1] * -temp;
            ix++;
          }
        }

        jy += n;
        jA += n;
      }
    }

    for (jy = 0; jy + 1 <= n; jy++) {
      if (ipiv->data[jy] != jy + 1) {
        temp = B->data[jy];
        B->data[jy] = B->data[ipiv->data[jy] - 1];
        B->data[ipiv->data[jy] - 1] = temp;
      }
    }

    emxFree_int32_T(&ipiv);
    for (k = 0; k + 1 <= n; k++) {
      c = n * k;
      if (B->data[k] != 0.0) {
        for (jy = k + 2; jy <= n; jy++) {
          B->data[jy - 1] -= B->data[k] * b_B->data[(jy + c) - 1];
        }
      }
    }

    for (k = n - 1; k + 1 > 0; k--) {
      c = n * k;
      if (B->data[k] != 0.0) {
        B->data[k] /= b_B->data[k + c];
        for (jy = 1; jy <= k; jy++) {
          B->data[jy - 1] -= B->data[k] * b_B->data[(jy + c) - 1];
        }
      }
    }

    emxFree_real_T(&b_B);
  } else {
    emxInit_real_T(&b_B, 1);
    i5 = b_B->size[0];
    b_B->size[0] = B->size[0];
    emxEnsureCapacity((emxArray__common *)b_B, i5, (int32_T)sizeof(real_T));
    jy = B->size[0] - 1;
    for (i5 = 0; i5 <= jy; i5++) {
      b_B->data[i5] = B->data[i5];
    }

    emxInit_real_T(&r11, 1);
    eml_qrsolve(A, b_B, r11);
    i5 = B->size[0];
    B->size[0] = r11->size[0];
    emxEnsureCapacity((emxArray__common *)B, i5, (int32_T)sizeof(real_T));
    emxFree_real_T(&b_B);
    jy = r11->size[0] - 1;
    for (i5 = 0; i5 <= jy; i5++) {
      B->data[i5] = r11->data[i5];
    }

    emxFree_real_T(&r11);
  }

  emxFree_real_T(&Y);
}

/* End of code generation (mldivide.cpp) */
