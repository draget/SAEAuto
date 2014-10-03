/*
 * checkpathcollision.cpp
 *
 * Code generation for function 'checkpathcollision'
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

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function [pathcollision] = checkpathcollision(arcob, pathqi, s)
 */
void checkpathcollision(const emxArray_real_T *arcob, const emxArray_real_T
  *pathqi, const emxArray_real_T *s, emxArray_boolean_T *pathcollision)
{
  emxArray_real_T *b_pathcollision;
  int32_T i15;
  int32_T n;
  int32_T br;
  int32_T i;
  emxArray_real_T *C;
  emxArray_real_T *a;
  emxArray_real_T *b;
  emxArray_real_T *r20;
  emxArray_real_T *r21;
  emxArray_real_T *r22;
  emxArray_real_T *r23;
  real_T b_arcob;
  real_T c_arcob;
  int32_T nm1;
  int32_T k;
  uint32_T unnamed_idx_1;
  int32_T ic;
  int32_T ar;
  int32_T ib;
  int32_T ia;
  b_emxInit_real_T(&b_pathcollision, 2);

  /* UNTITLED5 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'checkpathcollision:5' safety = 0; */
  /* safety margin to add to offset. */
  /* 'checkpathcollision:7' [~,nobstacles] = size(arcob); */
  /* 'checkpathcollision:8' [~,npaths] = size(pathqi); */
  /* 'checkpathcollision:9' pathcollision = zeros(1,npaths); */
  i15 = b_pathcollision->size[0] * b_pathcollision->size[1];
  b_pathcollision->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)b_pathcollision, i15, (int32_T)sizeof
                    (real_T));
  n = pathqi->size[1];
  i15 = b_pathcollision->size[0] * b_pathcollision->size[1];
  b_pathcollision->size[1] = n;
  emxEnsureCapacity((emxArray__common *)b_pathcollision, i15, (int32_T)sizeof
                    (real_T));
  br = pathqi->size[1] - 1;
  for (i15 = 0; i15 <= br; i15++) {
    b_pathcollision->data[i15] = 0.0;
  }

  /* 'checkpathcollision:11' for i = 1:nobstacles */
  i = 0;
  b_emxInit_real_T(&C, 2);
  b_emxInit_real_T(&a, 2);
  b_emxInit_real_T(&b, 2);
  emxInit_real_T(&r20, 1);
  emxInit_real_T(&r21, 1);
  emxInit_real_T(&r22, 1);
  emxInit_real_T(&r23, 1);
  while (i <= arcob->size[1] - 1) {
    /* 'checkpathcollision:12' obarcl = arcob(1,i); */
    /* 'checkpathcollision:13' oboff = arcob(2,i); */
    /* 'checkpathcollision:14' obrad = arcob(3,i); */
    /* 'checkpathcollision:16' if oboff >= 0 */
    if (arcob->data[1 + arcob->size[0] * i] >= 0.0) {
      /* 'checkpathcollision:17' pathcollision = pathcollision +... */
      /* 'checkpathcollision:18'             ((obarcl-obrad<s).*(s<obarcl+obrad))' *... */
      /* 'checkpathcollision:19'             ((oboff-obrad-safety<pathqi).*(pathqi<oboff+obrad+safety)); */
      b_arcob = arcob->data[arcob->size[0] * i] - arcob->data[2 + arcob->size[0]
        * i];
      i15 = r20->size[0];
      r20->size[0] = s->size[0];
      emxEnsureCapacity((emxArray__common *)r20, i15, (int32_T)sizeof(real_T));
      br = s->size[0] - 1;
      for (i15 = 0; i15 <= br; i15++) {
        r20->data[i15] = (real_T)(b_arcob < s->data[i15]);
      }

      b_arcob = arcob->data[arcob->size[0] * i] + arcob->data[2 + arcob->size[0]
        * i];
      i15 = r21->size[0];
      r21->size[0] = s->size[0];
      emxEnsureCapacity((emxArray__common *)r21, i15, (int32_T)sizeof(real_T));
      br = s->size[0] - 1;
      for (i15 = 0; i15 <= br; i15++) {
        r21->data[i15] = (real_T)(s->data[i15] < b_arcob);
      }

      i15 = a->size[0] * a->size[1];
      a->size[0] = 1;
      emxEnsureCapacity((emxArray__common *)a, i15, (int32_T)sizeof(real_T));
      n = r20->size[0];
      i15 = a->size[0] * a->size[1];
      a->size[1] = n;
      emxEnsureCapacity((emxArray__common *)a, i15, (int32_T)sizeof(real_T));
      br = r20->size[0] - 1;
      for (i15 = 0; i15 <= br; i15++) {
        a->data[i15] = r20->data[i15] * r21->data[i15];
      }

      i15 = b->size[0] * b->size[1];
      b->size[0] = pathqi->size[0];
      b->size[1] = pathqi->size[1];
      emxEnsureCapacity((emxArray__common *)b, i15, (int32_T)sizeof(real_T));
      b_arcob = arcob->data[1 + arcob->size[0] * i] - arcob->data[2 +
        arcob->size[0] * i];
      c_arcob = arcob->data[1 + arcob->size[0] * i] + arcob->data[2 +
        arcob->size[0] * i];
      br = pathqi->size[0] * pathqi->size[1] - 1;
      for (i15 = 0; i15 <= br; i15++) {
        b->data[i15] = (real_T)(b_arcob < pathqi->data[i15]) * (real_T)
          (pathqi->data[i15] < c_arcob);
      }

      if ((a->size[1] == 1) || (b->size[0] == 1)) {
        i15 = C->size[0] * C->size[1];
        C->size[0] = 1;
        C->size[1] = b->size[1];
        emxEnsureCapacity((emxArray__common *)C, i15, (int32_T)sizeof(real_T));
        br = b->size[1] - 1;
        for (i15 = 0; i15 <= br; i15++) {
          C->data[C->size[0] * i15] = 0.0;
          nm1 = a->size[1] - 1;
          for (n = 0; n <= nm1; n++) {
            C->data[C->size[0] * i15] += a->data[a->size[0] * n] * b->data[n +
              b->size[0] * i15];
          }
        }
      } else {
        k = a->size[1];
        unnamed_idx_1 = (uint32_T)b->size[1];
        i15 = C->size[0] * C->size[1];
        C->size[0] = 1;
        C->size[1] = (int32_T)unnamed_idx_1;
        emxEnsureCapacity((emxArray__common *)C, i15, (int32_T)sizeof(real_T));
        n = b->size[1];
        i15 = C->size[0] * C->size[1];
        C->size[0] = 1;
        C->size[1] = C->size[1];
        emxEnsureCapacity((emxArray__common *)C, i15, (int32_T)sizeof(real_T));
        br = C->size[1] - 1;
        for (i15 = 0; i15 <= br; i15++) {
          C->data[C->size[0] * i15] = 0.0;
        }

        if (n == 0) {
        } else {
          nm1 = n - 1;
          for (n = 0; n <= nm1; n++) {
            for (ic = n; ic + 1 <= n + 1; ic++) {
              C->data[ic] = 0.0;
            }
          }

          br = 0;
          for (n = 0; n <= nm1; n++) {
            ar = -1;
            i15 = br + k;
            for (ib = br; ib + 1 <= i15; ib++) {
              if (b->data[ib] != 0.0) {
                ia = ar;
                for (ic = n; ic + 1 <= n + 1; ic++) {
                  ia++;
                  C->data[ic] += b->data[ib] * a->data[ia];
                }
              }

              ar++;
            }

            br += k;
          }
        }
      }

      i15 = b_pathcollision->size[0] * b_pathcollision->size[1];
      b_pathcollision->size[0] = 1;
      b_pathcollision->size[1] = b_pathcollision->size[1];
      emxEnsureCapacity((emxArray__common *)b_pathcollision, i15, (int32_T)
                        sizeof(real_T));
      n = b_pathcollision->size[0];
      nm1 = b_pathcollision->size[1];
      br = n * nm1 - 1;
      for (i15 = 0; i15 <= br; i15++) {
        b_pathcollision->data[i15] += C->data[i15];
      }
    } else {
      /* 'checkpathcollision:20' else */
      /* 'checkpathcollision:21' pathcollision = pathcollision +... */
      /* 'checkpathcollision:22'             ((obarcl-obrad<s).*(s<obarcl+obrad))' *... */
      /* 'checkpathcollision:23'             ((oboff+obrad+safety>pathqi).*(pathqi>oboff-obrad-safety)); */
      b_arcob = arcob->data[arcob->size[0] * i] - arcob->data[2 + arcob->size[0]
        * i];
      i15 = r22->size[0];
      r22->size[0] = s->size[0];
      emxEnsureCapacity((emxArray__common *)r22, i15, (int32_T)sizeof(real_T));
      br = s->size[0] - 1;
      for (i15 = 0; i15 <= br; i15++) {
        r22->data[i15] = (real_T)(b_arcob < s->data[i15]);
      }

      b_arcob = arcob->data[arcob->size[0] * i] + arcob->data[2 + arcob->size[0]
        * i];
      i15 = r23->size[0];
      r23->size[0] = s->size[0];
      emxEnsureCapacity((emxArray__common *)r23, i15, (int32_T)sizeof(real_T));
      br = s->size[0] - 1;
      for (i15 = 0; i15 <= br; i15++) {
        r23->data[i15] = (real_T)(s->data[i15] < b_arcob);
      }

      i15 = a->size[0] * a->size[1];
      a->size[0] = 1;
      emxEnsureCapacity((emxArray__common *)a, i15, (int32_T)sizeof(real_T));
      n = r22->size[0];
      i15 = a->size[0] * a->size[1];
      a->size[1] = n;
      emxEnsureCapacity((emxArray__common *)a, i15, (int32_T)sizeof(real_T));
      br = r22->size[0] - 1;
      for (i15 = 0; i15 <= br; i15++) {
        a->data[i15] = r22->data[i15] * r23->data[i15];
      }

      i15 = b->size[0] * b->size[1];
      b->size[0] = pathqi->size[0];
      b->size[1] = pathqi->size[1];
      emxEnsureCapacity((emxArray__common *)b, i15, (int32_T)sizeof(real_T));
      b_arcob = arcob->data[1 + arcob->size[0] * i] + arcob->data[2 +
        arcob->size[0] * i];
      c_arcob = arcob->data[1 + arcob->size[0] * i] - arcob->data[2 +
        arcob->size[0] * i];
      br = pathqi->size[0] * pathqi->size[1] - 1;
      for (i15 = 0; i15 <= br; i15++) {
        b->data[i15] = (real_T)(b_arcob > pathqi->data[i15]) * (real_T)
          (pathqi->data[i15] > c_arcob);
      }

      if ((a->size[1] == 1) || (b->size[0] == 1)) {
        i15 = C->size[0] * C->size[1];
        C->size[0] = 1;
        C->size[1] = b->size[1];
        emxEnsureCapacity((emxArray__common *)C, i15, (int32_T)sizeof(real_T));
        br = b->size[1] - 1;
        for (i15 = 0; i15 <= br; i15++) {
          C->data[C->size[0] * i15] = 0.0;
          nm1 = a->size[1] - 1;
          for (n = 0; n <= nm1; n++) {
            C->data[C->size[0] * i15] += a->data[a->size[0] * n] * b->data[n +
              b->size[0] * i15];
          }
        }
      } else {
        k = a->size[1];
        unnamed_idx_1 = (uint32_T)b->size[1];
        i15 = C->size[0] * C->size[1];
        C->size[0] = 1;
        C->size[1] = (int32_T)unnamed_idx_1;
        emxEnsureCapacity((emxArray__common *)C, i15, (int32_T)sizeof(real_T));
        n = b->size[1];
        i15 = C->size[0] * C->size[1];
        C->size[0] = 1;
        C->size[1] = C->size[1];
        emxEnsureCapacity((emxArray__common *)C, i15, (int32_T)sizeof(real_T));
        br = C->size[1] - 1;
        for (i15 = 0; i15 <= br; i15++) {
          C->data[C->size[0] * i15] = 0.0;
        }

        if (n == 0) {
        } else {
          nm1 = n - 1;
          for (n = 0; n <= nm1; n++) {
            for (ic = n; ic + 1 <= n + 1; ic++) {
              C->data[ic] = 0.0;
            }
          }

          br = 0;
          for (n = 0; n <= nm1; n++) {
            ar = -1;
            i15 = br + k;
            for (ib = br; ib + 1 <= i15; ib++) {
              if (b->data[ib] != 0.0) {
                ia = ar;
                for (ic = n; ic + 1 <= n + 1; ic++) {
                  ia++;
                  C->data[ic] += b->data[ib] * a->data[ia];
                }
              }

              ar++;
            }

            br += k;
          }
        }
      }

      i15 = b_pathcollision->size[0] * b_pathcollision->size[1];
      b_pathcollision->size[0] = 1;
      b_pathcollision->size[1] = b_pathcollision->size[1];
      emxEnsureCapacity((emxArray__common *)b_pathcollision, i15, (int32_T)
                        sizeof(real_T));
      n = b_pathcollision->size[0];
      nm1 = b_pathcollision->size[1];
      br = n * nm1 - 1;
      for (i15 = 0; i15 <= br; i15++) {
        b_pathcollision->data[i15] += C->data[i15];
      }
    }

    i++;
  }

  emxFree_real_T(&r23);
  emxFree_real_T(&r22);
  emxFree_real_T(&r21);
  emxFree_real_T(&r20);
  emxFree_real_T(&b);
  emxFree_real_T(&a);
  emxFree_real_T(&C);

  /* 'checkpathcollision:28' pathcollision = pathcollision > 0; */
  i15 = pathcollision->size[0] * pathcollision->size[1];
  pathcollision->size[0] = 1;
  pathcollision->size[1] = b_pathcollision->size[1];
  emxEnsureCapacity((emxArray__common *)pathcollision, i15, (int32_T)sizeof
                    (boolean_T));
  br = b_pathcollision->size[0] * b_pathcollision->size[1] - 1;
  for (i15 = 0; i15 <= br; i15++) {
    pathcollision->data[i15] = (b_pathcollision->data[i15] > 0.0);
  }

  emxFree_real_T(&b_pathcollision);
}

/* End of code generation (checkpathcollision.cpp) */
