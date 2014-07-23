/*
 * adaptivegaus.cpp
 *
 * Code generation for function 'adaptivegaus'
 *
 * C source code generated on: Wed Jul 23 14:27:34 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "builddetailedbf.h"
#include "adaptivegaus.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

/*
 * function int = adaptivegaus(coefx,coefy,ti,ti1,tif,epsilon)
 */
real_T adaptivegaus(const real_T coefx[4], const real_T coefy[4], real_T ti,
                    real_T ti1, real_T tif, real_T epsilon)
{
  real_T b_int;
  real_T h;
  real_T c1;
  real_T c2;
  real_T fullint;
  real_T b_epsilon;
  real_T sp;
  real_T stack[400];
  real_T left;
  real_T right;
  real_T mid;
  real_T leftvalue;

  /* UNTITLED10 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'adaptivegaus:5' fullint = twoptgaus(coefx,coefy,ti,ti1,tif); */
  /* UNTITLED9 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'twoptgaus:5' h = ti1 - ti; */
  h = ti1 - ti;

  /* 'twoptgaus:7' c1=0.5*h*(1-1/sqrt(3)); */
  c1 = 0.5 * h * 0.42264973081037416;

  /* 'twoptgaus:8' c2=0.5*h*(1+1/sqrt(3)); */
  c2 = 0.5 * h * 1.5773502691896257;

  /* 'twoptgaus:10' int1 = sqrt((coefx(3)+2*coefx(2)*(ti+c1-tif)+3*coefx(1)*(ti+c1-tif)^2)^2 +... */
  /* 'twoptgaus:11'            (coefy(3)+2*coefy(2)*(ti+c1-tif)+3*coefy(1)*(ti+c1-tif)^2)^2); */
  /* 'twoptgaus:12' int2 = sqrt((coefx(3)+2*coefx(2)*(ti+c2-tif)+3*coefx(1)*(ti+c2-tif)^2)^2 +... */
  /* 'twoptgaus:13'            (coefy(3)+2*coefy(2)*(ti+c2-tif)+3*coefy(1)*(ti+c2-tif)^2)^2); */
  /* 'twoptgaus:15' int = 0.5*h*(int1 + int2); */
  /* 'adaptivegaus:7' int = subdivide(coefx,coefy,ti,ti1,tif,fullint,epsilon); */
  fullint = 0.5 * h * (sqrt(rt_powd_snf((coefx[2] + 2.0 * coefx[1] * ((ti + c1)
    - tif)) + 3.0 * coefx[0] * rt_powd_snf((ti + c1) - tif, 2.0), 2.0) +
    rt_powd_snf((coefy[2] + 2.0 * coefy[1] * ((ti + c1) - tif)) + 3.0 * coefy[0]
                * rt_powd_snf((ti + c1) - tif, 2.0), 2.0)) + sqrt(rt_powd_snf
    ((coefx[2] + 2.0 * coefx[1] * ((ti + c2) - tif)) + 3.0 * coefx[0] *
     rt_powd_snf((ti + c2) - tif, 2.0), 2.0) + rt_powd_snf((coefy[2] + 2.0 *
    coefy[1] * ((ti + c2) - tif)) + 3.0 * coefy[0] * rt_powd_snf((ti + c2) - tif,
    2.0), 2.0)));
  b_epsilon = 0.0001;

  /* UNTITLED11 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* 'subdivide:5' int = 0; */
  b_int = 0.0;

  /* integral result */
  /* 'subdivide:6' sp = 1; */
  sp = 1.0;

  /* stack pointer */
  /* 'subdivide:7' stack = zeros(100,4); */
  memset(&stack[0], 0, 400U * sizeof(real_T));

  /* 'subdivide:8' left = ti; */
  left = ti;

  /* left */
  /* 'subdivide:9' right = ti1; */
  right = ti1;

  /* right */
  /* 'subdivide:11' while sp > 0 */
  while (sp > 0.0) {
    /* 'subdivide:13' mid = (left + right)/2; */
    mid = (left + right) / 2.0;

    /* 'subdivide:14' leftvalue = twoptgaus(coefx,coefy,left,mid,tif); */
    /* UNTITLED9 Summary of this function goes here */
    /*    Detailed explanation goes here */
    /* 'twoptgaus:5' h = ti1 - ti; */
    h = mid - left;

    /* 'twoptgaus:7' c1=0.5*h*(1-1/sqrt(3)); */
    c1 = 0.5 * h * 0.42264973081037416;

    /* 'twoptgaus:8' c2=0.5*h*(1+1/sqrt(3)); */
    c2 = 0.5 * h * 1.5773502691896257;

    /* 'twoptgaus:10' int1 = sqrt((coefx(3)+2*coefx(2)*(ti+c1-tif)+3*coefx(1)*(ti+c1-tif)^2)^2 +... */
    /* 'twoptgaus:11'            (coefy(3)+2*coefy(2)*(ti+c1-tif)+3*coefy(1)*(ti+c1-tif)^2)^2); */
    /* 'twoptgaus:12' int2 = sqrt((coefx(3)+2*coefx(2)*(ti+c2-tif)+3*coefx(1)*(ti+c2-tif)^2)^2 +... */
    /* 'twoptgaus:13'            (coefy(3)+2*coefy(2)*(ti+c2-tif)+3*coefy(1)*(ti+c2-tif)^2)^2); */
    /* 'twoptgaus:15' int = 0.5*h*(int1 + int2); */
    leftvalue = 0.5 * h * (sqrt(rt_powd_snf((coefx[2] + 2.0 * coefx[1] * ((left
      + c1) - tif)) + 3.0 * coefx[0] * rt_powd_snf((left + c1) - tif, 2.0), 2.0)
      + rt_powd_snf((coefy[2] + 2.0 * coefy[1] * ((left + c1) - tif)) + 3.0 *
                    coefy[0] * rt_powd_snf((left + c1) - tif, 2.0), 2.0)) + sqrt
                           (rt_powd_snf((coefx[2] + 2.0 * coefx[1] * ((left + c2)
      - tif)) + 3.0 * coefx[0] * rt_powd_snf((left + c2) - tif, 2.0), 2.0) +
      rt_powd_snf((coefy[2] + 2.0 * coefy[1] * ((left + c2) - tif)) + 3.0 *
                  coefy[0] * rt_powd_snf((left + c2) - tif, 2.0), 2.0)));

    /* 'subdivide:15' rightvalue = twoptgaus(coefx,coefy,mid,right,tif); */
    /* UNTITLED9 Summary of this function goes here */
    /*    Detailed explanation goes here */
    /* 'twoptgaus:5' h = ti1 - ti; */
    h = right - mid;

    /* 'twoptgaus:7' c1=0.5*h*(1-1/sqrt(3)); */
    c1 = 0.5 * h * 0.42264973081037416;

    /* 'twoptgaus:8' c2=0.5*h*(1+1/sqrt(3)); */
    c2 = 0.5 * h * 1.5773502691896257;

    /* 'twoptgaus:10' int1 = sqrt((coefx(3)+2*coefx(2)*(ti+c1-tif)+3*coefx(1)*(ti+c1-tif)^2)^2 +... */
    /* 'twoptgaus:11'            (coefy(3)+2*coefy(2)*(ti+c1-tif)+3*coefy(1)*(ti+c1-tif)^2)^2); */
    /* 'twoptgaus:12' int2 = sqrt((coefx(3)+2*coefx(2)*(ti+c2-tif)+3*coefx(1)*(ti+c2-tif)^2)^2 +... */
    /* 'twoptgaus:13'            (coefy(3)+2*coefy(2)*(ti+c2-tif)+3*coefy(1)*(ti+c2-tif)^2)^2); */
    /* 'twoptgaus:15' int = 0.5*h*(int1 + int2); */
    h = 0.5 * h * (sqrt(rt_powd_snf((coefx[2] + 2.0 * coefx[1] * ((mid + c1) -
      tif)) + 3.0 * coefx[0] * rt_powd_snf((mid + c1) - tif, 2.0), 2.0) +
                        rt_powd_snf((coefy[2] + 2.0 * coefy[1] * ((mid + c1) -
      tif)) + 3.0 * coefy[0] * rt_powd_snf((mid + c1) - tif, 2.0), 2.0)) + sqrt
                   (rt_powd_snf((coefx[2] + 2.0 * coefx[1] * ((mid + c2) - tif))
      + 3.0 * coefx[0] * rt_powd_snf((mid + c2) - tif, 2.0), 2.0) + rt_powd_snf
                    ((coefy[2] + 2.0 * coefy[1] * ((mid + c2) - tif)) + 3.0 *
                     coefy[0] * rt_powd_snf((mid + c2) - tif, 2.0), 2.0)));

    /* 'subdivide:17' if abs(fullint-(leftvalue + rightvalue)) > epsilon */
    if (fabs(fullint - (leftvalue + h)) > b_epsilon) {
      /* 'subdivide:19' sp = sp + 1; */
      sp++;

      /* 'subdivide:20' stack(sp,1) = mid; */
      stack[(int32_T)sp - 1] = mid;

      /* 'subdivide:21' stack(sp,2) = right; */
      stack[(int32_T)sp + 99] = right;

      /* 'subdivide:22' stack(sp,3) = rightvalue; */
      stack[(int32_T)sp + 199] = h;

      /* 'subdivide:23' stack(sp,4) = epsilon/2; */
      stack[(int32_T)sp + 299] = b_epsilon / 2.0;

      /* 'subdivide:25' right = mid; */
      right = mid;

      /* 'subdivide:26' fullint = leftvalue; */
      fullint = leftvalue;

      /* 'subdivide:27' epsilon = epsilon/2; */
      b_epsilon /= 2.0;
    } else {
      /* 'subdivide:29' else */
      /* 'subdivide:30' int = int + leftvalue + rightvalue; */
      b_int = (b_int + leftvalue) + h;

      /* 'subdivide:32' left = stack(sp,1); */
      left = stack[(int32_T)sp - 1];

      /* 'subdivide:33' right = stack(sp,2); */
      right = stack[(int32_T)sp + 99];

      /* 'subdivide:34' fullint = stack(sp,3); */
      fullint = stack[(int32_T)sp + 199];

      /* 'subdivide:35' epsilon = stack(sp,4); */
      b_epsilon = stack[(int32_T)sp + 299];

      /* 'subdivide:36' sp = sp - 1; */
      sp--;
    }
  }

  return b_int;
}

/* End of code generation (adaptivegaus.cpp) */
