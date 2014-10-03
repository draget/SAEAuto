/*
 * oblocalize.cpp
 *
 * Code generation for function 'oblocalize'
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
 * function arcob = oblocalize(scoefx,scoefy,si,ob,sguess,epsilon)
 */
void oblocalize(const emxArray_real_T *scoefx, const emxArray_real_T *scoefy,
                const emxArray_real_T *si, const emxArray_real_T *ob, real_T
                sguess, real_T epsilon, emxArray_real_T *arcob)
{
  int32_T i;
  int32_T loop_ub;
  real_T unusedU2;
  real_T unusedU1;
  real_T d0;

  /* UNTITLED4 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'oblocalize:5' [~,c] = size(ob); */
  /* 'oblocalize:6' arcob = ob; */
  i = arcob->size[0] * arcob->size[1];
  arcob->size[0] = 3;
  arcob->size[1] = ob->size[1];
  emxEnsureCapacity((emxArray__common *)arcob, i, (int32_T)sizeof(real_T));
  loop_ub = ob->size[0] * ob->size[1] - 1;
  for (i = 0; i <= loop_ub; i++) {
    arcob->data[i] = ob->data[i];
  }

  /* 'oblocalize:8' for i=1:c */
  for (i = 0; i <= ob->size[1] - 1; i++) {
    /* 'oblocalize:9' [arcob(1,i),arcob(2,i), ~, ~] = localize(scoefx,scoefy,si,ob(1,i),ob(2,i),sguess,epsilon); */
    localize(scoefx, scoefy, si, ob->data[ob->size[0] * i], ob->data[1 +
             ob->size[0] * i], sguess, epsilon, &arcob->data[arcob->size[0] * i],
             &d0, &unusedU1, &unusedU2);
    arcob->data[1 + arcob->size[0] * i] = d0;
  }
}

/* End of code generation (oblocalize.cpp) */
