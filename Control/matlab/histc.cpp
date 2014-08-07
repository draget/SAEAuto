/*
 * histc.cpp
 *
 * Code generation for function 'histc'
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

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
static int32_T findbin(real_T x, const emxArray_real_T *bin_edges);

/* Function Definitions */

/*
 *
 */
static int32_T findbin(real_T x, const emxArray_real_T *bin_edges)
{
  int32_T k;
  uint32_T low_i;
  uint32_T low_ip1;
  uint32_T high_i;
  uint32_T mid_i;
  k = 0;
  if ((!(bin_edges->size[0] == 0)) && (!rtIsNaN(x))) {
    if ((x >= bin_edges->data[0]) && (x < bin_edges->data[bin_edges->size[0] - 1]))
    {
      low_i = 1U;
      low_ip1 = 2U;
      high_i = (uint32_T)bin_edges->size[0];
      while (high_i > low_ip1) {
        mid_i = (low_i >> 1U) + (high_i >> 1U);
        if (((low_i & 1U) == 1U) && ((high_i & 1U) == 1U)) {
          mid_i++;
        }

        if (x >= bin_edges->data[(int32_T)mid_i - 1]) {
          low_i = mid_i;
          low_ip1 = mid_i + 1U;
        } else {
          high_i = mid_i;
        }
      }

      k = (int32_T)low_i;
    }

    if (x == bin_edges->data[bin_edges->size[0] - 1]) {
      k = bin_edges->size[0];
    }
  }

  return k;
}

/*
 *
 */
void b_histc(real_T X, const emxArray_real_T *edges, emxArray_real_T *N, real_T *
             BIN)
{
  uint32_T outsize[2];
  int32_T bin;
  int32_T loop_ub;
  boolean_T guard1 = FALSE;
  int32_T exitg1;
  for (bin = 0; bin < 2; bin++) {
    outsize[bin] = 1U;
  }

  outsize[0] = 1U;
  outsize[1] = (uint32_T)edges->size[0];
  bin = N->size[0] * N->size[1];
  N->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)N, bin, (int32_T)sizeof(real_T));
  bin = N->size[0] * N->size[1];
  N->size[1] = (int32_T)outsize[1];
  emxEnsureCapacity((emxArray__common *)N, bin, (int32_T)sizeof(real_T));
  loop_ub = (int32_T)outsize[1] - 1;
  for (bin = 0; bin <= loop_ub; bin++) {
    N->data[bin] = 0.0;
  }

  guard1 = FALSE;
  if (edges->size[0] > 1) {
    bin = 0;
    do {
      exitg1 = 0;
      if (bin <= edges->size[0] - 2) {
        if (edges->data[bin + 1] < edges->data[bin]) {
          bin = N->size[0] * N->size[1];
          N->size[0] = 1;
          emxEnsureCapacity((emxArray__common *)N, bin, (int32_T)sizeof(real_T));
          bin = N->size[0] * N->size[1];
          N->size[1] = (int32_T)outsize[1];
          emxEnsureCapacity((emxArray__common *)N, bin, (int32_T)sizeof(real_T));
          loop_ub = (int32_T)outsize[1] - 1;
          for (bin = 0; bin <= loop_ub; bin++) {
            N->data[bin] = rtNaN;
          }

          *BIN = rtNaN;
          exitg1 = 1;
        } else {
          bin++;
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
    bin = findbin(X, edges);
    if (bin > 0) {
      N->data[bin - 1] = 1.0;
    }

    *BIN = (real_T)bin;
  }
}

/*
 *
 */
void c_histc(const real_T X[4], const emxArray_real_T *edges, emxArray_real_T *N,
             real_T BIN[4])
{
  uint32_T outsize[2];
  int32_T i;
  int32_T loop_ub;
  boolean_T guard1 = FALSE;
  int32_T exitg1;
  int32_T bin;
  for (i = 0; i < 2; i++) {
    outsize[i] = (uint32_T)(4 + -3 * i);
  }

  outsize[0] = (uint32_T)edges->size[0];
  outsize[1] = 1U;
  i = N->size[0];
  N->size[0] = (int32_T)outsize[0];
  emxEnsureCapacity((emxArray__common *)N, i, (int32_T)sizeof(real_T));
  loop_ub = (int32_T)outsize[0] - 1;
  for (i = 0; i <= loop_ub; i++) {
    N->data[i] = 0.0;
  }

  for (i = 0; i < 4; i++) {
    BIN[i] = 0.0;
  }

  guard1 = FALSE;
  if (edges->size[0] > 1) {
    i = 0;
    do {
      exitg1 = 0;
      if (i <= edges->size[0] - 2) {
        if (edges->data[i + 1] < edges->data[i]) {
          i = N->size[0];
          N->size[0] = (int32_T)outsize[0];
          emxEnsureCapacity((emxArray__common *)N, i, (int32_T)sizeof(real_T));
          loop_ub = (int32_T)outsize[0] - 1;
          for (i = 0; i <= loop_ub; i++) {
            N->data[i] = rtNaN;
          }

          for (i = 0; i < 4; i++) {
            BIN[i] = rtNaN;
          }

          exitg1 = 1;
        } else {
          i++;
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
    i = 0;
    for (loop_ub = 0; loop_ub < 4; loop_ub++) {
      bin = findbin(X[i], edges);
      if (bin > 0) {
        N->data[bin - 1]++;
      }

      BIN[i] = (real_T)bin;
      i++;
    }
  }
}

/*
 *
 */
void histc(const emxArray_real_T *X, const emxArray_real_T *edges,
           emxArray_real_T *N, emxArray_real_T *BIN)
{
  uint32_T outsize_idx_0;
  int32_T xind;
  int32_T loop_ub;
  uint32_T unnamed_idx_0;
  boolean_T guard1 = FALSE;
  int32_T exitg1;
  int32_T bin;
  int32_T binind;
  outsize_idx_0 = (uint32_T)edges->size[0];
  xind = N->size[0];
  N->size[0] = (int32_T)outsize_idx_0;
  emxEnsureCapacity((emxArray__common *)N, xind, (int32_T)sizeof(real_T));
  loop_ub = (int32_T)outsize_idx_0 - 1;
  for (xind = 0; xind <= loop_ub; xind++) {
    N->data[xind] = 0.0;
  }

  unnamed_idx_0 = (uint32_T)X->size[0];
  xind = BIN->size[0];
  BIN->size[0] = (int32_T)unnamed_idx_0;
  emxEnsureCapacity((emxArray__common *)BIN, xind, (int32_T)sizeof(real_T));
  loop_ub = (int32_T)unnamed_idx_0 - 1;
  for (xind = 0; xind <= loop_ub; xind++) {
    BIN->data[xind] = 0.0;
  }

  guard1 = FALSE;
  if (edges->size[0] > 1) {
    xind = 0;
    do {
      exitg1 = 0;
      if (xind <= edges->size[0] - 2) {
        if (edges->data[xind + 1] < edges->data[xind]) {
          xind = N->size[0];
          N->size[0] = (int32_T)outsize_idx_0;
          emxEnsureCapacity((emxArray__common *)N, xind, (int32_T)sizeof(real_T));
          loop_ub = (int32_T)outsize_idx_0 - 1;
          for (xind = 0; xind <= loop_ub; xind++) {
            N->data[xind] = rtNaN;
          }

          xind = BIN->size[0];
          BIN->size[0] = (int32_T)unnamed_idx_0;
          emxEnsureCapacity((emxArray__common *)BIN, xind, (int32_T)sizeof
                            (real_T));
          loop_ub = (int32_T)unnamed_idx_0 - 1;
          for (xind = 0; xind <= loop_ub; xind++) {
            BIN->data[xind] = rtNaN;
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
    for (loop_ub = 0; loop_ub <= X->size[0] - 1; loop_ub++) {
      bin = findbin(X->data[xind], edges);
      if (bin > 0) {
        binind = bin - 1;
        N->data[binind]++;
      }

      BIN->data[xind] = (real_T)bin;
      xind++;
    }
  }
}

/* End of code generation (histc.cpp) */
