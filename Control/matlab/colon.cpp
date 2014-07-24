/*
 * colon.cpp
 *
 * Code generation for function 'colon'
 *
 * C source code generated on: Wed Jul 23 22:33:34 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "builddetailedbf.h"
#include "colon.h"
#include "matlab_emxutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 *
 */
void eml_signed_integer_colon(int32_T b, emxArray_int32_T *y)
{
  int32_T yk;
  int32_T k;
  yk = y->size[0] * y->size[1];
  y->size[0] = 1;
  y->size[1] = b;
  emxEnsureCapacity((emxArray__common *)y, yk, (int32_T)sizeof(int32_T));
  y->data[0] = 1;
  yk = 1;
  for (k = 2; k <= b; k++) {
    yk++;
    y->data[k - 1] = yk;
  }
}

/* End of code generation (colon.cpp) */
