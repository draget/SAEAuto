/*
 * equatesafetycost.cpp
 *
 * Code generation for function 'equatesafetycost'
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
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function safetycost = equatesafetycost(mangran,pathcollision)
 */
void equatesafetycost(real_T mangran, const emxArray_boolean_T *pathcollision,
                      emxArray_real_T *safetycost)
{
  emxArray_real_T *g;
  int32_T vlen;
  int32_T k;
  int32_T i;
  emxArray_real_T *x;
  real_T y;
  b_emxInit_real_T(&g, 2);

  /* UNTITLED7 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'equatesafetycost:4' npaths = length(pathcollision); */
  /* 'equatesafetycost:5' delq = mangran; */
  /* 'equatesafetycost:6' sigma = 1; */
  /* 'equatesafetycost:7' g = zeros(npaths,npaths); */
  vlen = pathcollision->size[1];
  k = g->size[0] * g->size[1];
  g->size[0] = vlen;
  emxEnsureCapacity((emxArray__common *)g, k, (int32_T)sizeof(real_T));
  vlen = pathcollision->size[1];
  k = g->size[0] * g->size[1];
  g->size[1] = vlen;
  emxEnsureCapacity((emxArray__common *)g, k, (int32_T)sizeof(real_T));
  vlen = pathcollision->size[1] * pathcollision->size[1] - 1;
  for (k = 0; k <= vlen; k++) {
    g->data[k] = 0.0;
  }

  /* 'equatesafetycost:8' for i=1:npaths */
  for (i = 0; i <= pathcollision->size[1] - 1; i++) {
    /* 'equatesafetycost:9' for k=1:npaths */
    for (k = 0; k <= pathcollision->size[1] - 1; k++) {
      /* 'equatesafetycost:10' g(i,k) = (1/(sqrt(2*pi)*sigma))*exp(-((delq.*(i-k))^2)/(2*sigma^2)); */
      g->data[i + g->size[0] * k] = 0.3989422804014327 * exp(-rt_powd_snf
        (mangran * (real_T)(i - k), 2.0) / 2.0);
    }
  }

  /* 'equatesafetycost:13' safetycost = zeros(1,npaths); */
  k = safetycost->size[0] * safetycost->size[1];
  safetycost->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)safetycost, k, (int32_T)sizeof(real_T));
  vlen = pathcollision->size[1];
  k = safetycost->size[0] * safetycost->size[1];
  safetycost->size[1] = vlen;
  emxEnsureCapacity((emxArray__common *)safetycost, k, (int32_T)sizeof(real_T));
  vlen = pathcollision->size[1] - 1;
  for (k = 0; k <= vlen; k++) {
    safetycost->data[k] = 0.0;
  }

  /* 'equatesafetycost:14' for i=1:npaths */
  i = 0;
  b_emxInit_real_T(&x, 2);
  while (i <= pathcollision->size[1] - 1) {
    /* 'equatesafetycost:15' gi = g(i,:); */
    /* 'equatesafetycost:16' safetycost(i) = sum(pathcollision.*gi); */
    k = x->size[0] * x->size[1];
    x->size[0] = 1;
    x->size[1] = pathcollision->size[1];
    emxEnsureCapacity((emxArray__common *)x, k, (int32_T)sizeof(real_T));
    vlen = pathcollision->size[1] - 1;
    for (k = 0; k <= vlen; k++) {
      x->data[x->size[0] * k] = (real_T)pathcollision->data[pathcollision->size
        [0] * k] * g->data[i + g->size[0] * k];
    }

    if (x->size[1] == 0) {
      y = 0.0;
    } else {
      vlen = x->size[1];
      y = x->data[0];
      for (k = 2; k <= vlen; k++) {
        y += x->data[k - 1];
      }
    }

    safetycost->data[i] = y;
    i++;
  }

  emxFree_real_T(&x);
  emxFree_real_T(&g);
}

/* End of code generation (equatesafetycost.cpp) */
