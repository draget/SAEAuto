/*
 * buildmanouvers.cpp
 *
 * Code generation for function 'buildmanouvers'
 *
 * C source code generated on: Mon Sep  1 19:20:44 2014
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "arclengthcurve.h"
#include "buildbfcurvature.h"
#include "builddetailedbf.h"
#include "buildmanouvers.h"
#include "checkpathcollision.h"
#include "equateoffsetcost.h"
#include "equatesafetycost.h"
#include "evalheading.h"
#include "localize.h"
#include "mincost.h"
#include "oblocalize.h"
#include "parevalspline.h"
#include "matlab_emxutil.h"
#include "cumtrapz.h"
#include "rdivide.h"
#include "power.h"
#include "linspace.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
static void eml_null_assignment(emxArray_real_T *x, const emxArray_real_T *idx);
static real_T rt_roundd_snf(real_T u);

/* Function Definitions */

/*
 *
 */
static void eml_null_assignment(emxArray_real_T *x, const emxArray_real_T *idx)
{
  int32_T ncols;
  real_T d3;
  int32_T j;
  int32_T i20;
  int32_T i;
  emxArray_boolean_T *b;
  int32_T nb;
  int32_T k;
  int32_T i21;
  emxArray_real_T *b_x;
  if (idx->size[1] == 1) {
    ncols = x->size[1] - 1;
    d3 = rt_roundd_snf(idx->data[0]);
    if (d3 < 2.147483648E+9) {
      if (d3 >= -2.147483648E+9) {
        j = (int32_T)d3;
      } else {
        j = MIN_int32_T;
      }
    } else if (d3 >= 2.147483648E+9) {
      j = MAX_int32_T;
    } else {
      j = 0;
    }

    while (j <= ncols) {
      i20 = x->size[0];
      for (i = 0; i + 1 <= i20; i++) {
        x->data[i + x->size[0] * (j - 1)] = x->data[i + x->size[0] * j];
      }

      j++;
    }
  } else {
    b_emxInit_boolean_T(&b, 2);
    i20 = b->size[0] * b->size[1];
    b->size[0] = 1;
    emxEnsureCapacity((emxArray__common *)b, i20, (int32_T)sizeof(boolean_T));
    nb = x->size[1];
    i20 = b->size[0] * b->size[1];
    b->size[1] = nb;
    emxEnsureCapacity((emxArray__common *)b, i20, (int32_T)sizeof(boolean_T));
    nb = x->size[1] - 1;
    for (i20 = 0; i20 <= nb; i20++) {
      b->data[i20] = FALSE;
    }

    for (k = 1; k <= idx->size[1]; k++) {
      b->data[(int32_T)idx->data[k - 1] - 1] = TRUE;
    }

    nb = 0;
    for (k = 1; k <= b->size[1]; k++) {
      j = (int32_T)b->data[k - 1];
      nb += j;
    }

    ncols = x->size[1] - nb;
    nb = b->size[1];
    j = 0;
    i20 = x->size[1];
    for (k = 1; k <= i20; k++) {
      if ((k > nb) || (!b->data[k - 1])) {
        i21 = x->size[0];
        for (i = 0; i + 1 <= i21; i++) {
          x->data[i + x->size[0] * j] = x->data[i + x->size[0] * (k - 1)];
        }

        j++;
      }
    }

    emxFree_boolean_T(&b);
  }

  if (1 > ncols) {
    ncols = 0;
  }

  b_emxInit_real_T(&b_x, 2);
  i = x->size[0];
  i20 = b_x->size[0] * b_x->size[1];
  b_x->size[0] = i;
  b_x->size[1] = ncols;
  emxEnsureCapacity((emxArray__common *)b_x, i20, (int32_T)sizeof(real_T));
  nb = ncols - 1;
  for (i20 = 0; i20 <= nb; i20++) {
    j = i - 1;
    for (i21 = 0; i21 <= j; i21++) {
      b_x->data[i21 + b_x->size[0] * i20] = x->data[i21 + x->size[0] * i20];
    }
  }

  i20 = x->size[0] * x->size[1];
  x->size[0] = b_x->size[0];
  x->size[1] = b_x->size[1];
  emxEnsureCapacity((emxArray__common *)x, i20, (int32_T)sizeof(real_T));
  nb = b_x->size[1] - 1;
  for (i20 = 0; i20 <= nb; i20++) {
    j = b_x->size[0] - 1;
    for (i21 = 0; i21 <= j; i21++) {
      x->data[i21 + x->size[0] * i20] = b_x->data[i21 + b_x->size[0] * i20];
    }
  }

  emxFree_real_T(&b_x);
}

static real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

/*
 * function [mxi,myi,pathki,pathqi,dthetai,mans] = buildmanouvers(scp,cpdistance,px,py,...
 *     maxlatoffset,mangran,manlength,mincurverad,thetadiff,...
 *     ss,sindex,kk,paththeta)
 */
void buildmanouvers(real_T scp, real_T cpdistance, real_T px, real_T py, real_T
                    maxlatoffset, real_T mangran, real_T manlength, real_T
                    mincurverad, real_T thetadiff, const emxArray_real_T *ss,
                    real_T sindex, const emxArray_real_T *kk, real_T paththeta,
                    emxArray_real_T *mxi, emxArray_real_T *myi, emxArray_real_T *
                    pathki, emxArray_real_T *pathqi, emxArray_real_T *dthetai,
                    emxArray_real_T *mans)
{
  emxArray_real_T *qflist;
  int32_T i10;
  int32_T r1;
  int32_T r2;
  emxArray_real_T *r11;
  real_T sf;
  real_T thetadiffr;
  real_T nsmallsegments;
  emxArray_real_T *r12;
  emxArray_int32_T *r13;
  emxArray_real_T *r14;
  emxArray_real_T *r15;
  emxArray_int32_T *r16;
  real_T c;
  emxArray_int32_T *r17;
  emxArray_real_T *b_kk;
  emxArray_int32_T *r18;
  emxArray_int32_T *r19;
  emxArray_real_T *c_kk;
  emxArray_int32_T *r20;
  emxArray_int32_T *r21;
  emxArray_real_T *d_kk;
  emxArray_int32_T *r22;
  emxArray_int32_T *r23;
  emxArray_real_T *e_kk;
  emxArray_int32_T *r24;
  emxArray_int32_T *r25;
  emxArray_real_T *f_kk;
  emxArray_int32_T *r26;
  emxArray_int32_T *r27;
  emxArray_real_T *g_kk;
  emxArray_int32_T *r28;
  emxArray_int32_T *r29;
  emxArray_real_T *h_kk;
  emxArray_int32_T *r30;
  emxArray_int32_T *r31;
  emxArray_real_T *i_kk;
  emxArray_int32_T *r32;
  emxArray_int32_T *r33;
  emxArray_real_T *j_kk;
  emxArray_int32_T *r34;
  emxArray_int32_T *r35;
  emxArray_real_T *k_kk;
  emxArray_int32_T *r36;
  emxArray_int32_T *r37;
  emxArray_real_T *l_kk;
  emxArray_int32_T *r38;
  emxArray_int32_T *r39;
  emxArray_real_T *m_kk;
  emxArray_int32_T *r40;
  emxArray_int32_T *r41;
  emxArray_real_T *n_kk;
  emxArray_int32_T *r42;
  emxArray_int32_T *r43;
  emxArray_real_T *o_kk;
  emxArray_int32_T *r44;
  emxArray_int32_T *r45;
  emxArray_real_T *p_kk;
  emxArray_real_T *removeindex;
  int32_T b_index;
  emxArray_real_T *q;
  emxArray_real_T *dq;
  emxArray_real_T *S;
  emxArray_real_T *Q;
  emxArray_real_T *theta;
  emxArray_real_T *my;
  emxArray_real_T *b;
  emxArray_boolean_T *x;
  emxArray_real_T *r46;
  emxArray_real_T *b_ss;
  emxArray_real_T *b_Q;
  emxArray_real_T *c_ss;
  emxArray_real_T *c_Q;
  emxArray_real_T *d_ss;
  emxArray_real_T *r47;
  emxArray_real_T *r48;
  emxArray_real_T *e_ss;
  emxArray_real_T *f_ss;
  emxArray_real_T *g_ss;
  emxArray_int32_T *r49;
  emxArray_int32_T *r50;
  emxArray_int32_T *r51;
  emxArray_int32_T *r52;
  emxArray_int32_T *r53;
  emxArray_real_T *q_kk;
  emxArray_int32_T *r54;
  emxArray_int32_T *r55;
  emxArray_int32_T *r56;
  emxArray_int32_T *r57;
  emxArray_int32_T *r58;
  emxArray_int32_T *r59;
  emxArray_int32_T *r60;
  emxArray_int32_T *r61;
  emxArray_int32_T *r62;
  emxArray_int32_T *r63;
  emxArray_int32_T *r64;
  emxArray_int32_T *r65;
  emxArray_int32_T *r66;
  emxArray_int32_T *r67;
  emxArray_int32_T *r68;
  emxArray_int32_T *r69;
  emxArray_int32_T *r70;
  emxArray_real_T *b_removeindex;
  emxArray_real_T *c_removeindex;
  real_T A[4];
  real_T B[2];
  real_T a22;
  real_T mcoef_idx_1;
  boolean_T invalid;
  int32_T exitg3;
  boolean_T guard1 = FALSE;
  real_T y;
  uint32_T unnamed_idx_0;
  int32_T exitg2;
  int32_T exitg1;
  b_emxInit_real_T(&qflist, 2);

  /* codegen */
  /* UNTITLED9 Summary of this function goes here */
  /*    Detailed explanation goes here */
  /* manouver common parameters */
  /* 'buildmanouvers:9' qi = cpdistance; */
  /* initial offset */
  /* 'buildmanouvers:10' qflist = qi + linspace(maxlatoffset,-maxlatoffset,ceil(2*maxlatoffset/mangran) + 1); */
  linspace(maxlatoffset, -maxlatoffset, ceil(2.0 * maxlatoffset / mangran) + 1.0,
           qflist);
  i10 = qflist->size[0] * qflist->size[1];
  qflist->size[0] = 1;
  qflist->size[1] = qflist->size[1];
  emxEnsureCapacity((emxArray__common *)qflist, i10, (int32_T)sizeof(real_T));
  r1 = qflist->size[0];
  r2 = qflist->size[1];
  r1 = r1 * r2 - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    qflist->data[i10] += cpdistance;
  }

  emxInit_real_T(&r11, 1);

  /* final offsets */
  /* 'buildmanouvers:11' sii = scp; */
  /* base manouver off point on base curve */
  /* 'buildmanouvers:12' sf = sii + manlength; */
  sf = scp + manlength;

  /* manouver  length */
  /* 'buildmanouvers:13' thetadiffr = thetadiff/360*(2*pi); */
  thetadiffr = thetadiff / 360.0 * 6.2831853071795862;

  /* heading angle, path angle difference */
  /* extract a list of arc lengths and baseframe curvature for the lenght of the manouver */
  /* 'buildmanouvers:17' smallsegmentlength = ss(2); */
  /* 'buildmanouvers:18' nsmallsegments = ceil(manlength/smallsegmentlength); */
  nsmallsegments = ceil(manlength / ss->data[1]);

  /* 'buildmanouvers:19' mans = ss(sindex:sindex+nsmallsegments); */
  i10 = r11->size[0];
  r11->size[0] = (int32_T)nsmallsegments + 1;
  emxEnsureCapacity((emxArray__common *)r11, i10, (int32_T)sizeof(real_T));
  for (i10 = 0; i10 <= (int32_T)nsmallsegments; i10++) {
    r11->data[i10] = (real_T)i10;
  }

  b_emxInit_real_T(&r12, 2);
  i10 = r12->size[0] * r12->size[1];
  r12->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)r12, i10, (int32_T)sizeof(real_T));
  r1 = r11->size[0];
  i10 = r12->size[0] * r12->size[1];
  r12->size[1] = r1;
  emxEnsureCapacity((emxArray__common *)r12, i10, (int32_T)sizeof(real_T));
  r1 = r11->size[0] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r12->data[i10] = sindex + r11->data[i10];
  }

  emxFree_real_T(&r11);
  emxInit_int32_T(&r13, 2);
  i10 = r13->size[0] * r13->size[1];
  r13->size[0] = 1;
  r13->size[1] = r12->size[1];
  emxEnsureCapacity((emxArray__common *)r13, i10, (int32_T)sizeof(int32_T));
  r1 = r12->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r13->data[r13->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
  }

  i10 = mans->size[0];
  mans->size[0] = r13->size[1];
  emxEnsureCapacity((emxArray__common *)mans, i10, (int32_T)sizeof(real_T));
  r1 = r13->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    mans->data[i10] = ss->data[(int32_T)r12->data[i10] - 1];
  }

  emxFree_int32_T(&r13);
  emxInit_real_T(&r14, 1);

  /* 'buildmanouvers:20' mank = kk(sindex:sindex+nsmallsegments); */
  i10 = r14->size[0];
  r14->size[0] = (int32_T)nsmallsegments + 1;
  emxEnsureCapacity((emxArray__common *)r14, i10, (int32_T)sizeof(real_T));
  for (i10 = 0; i10 <= (int32_T)nsmallsegments; i10++) {
    r14->data[i10] = (real_T)i10;
  }

  b_emxInit_real_T(&r15, 2);
  i10 = r15->size[0] * r15->size[1];
  r15->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)r15, i10, (int32_T)sizeof(real_T));
  r1 = r14->size[0];
  i10 = r15->size[0] * r15->size[1];
  r15->size[1] = r1;
  emxEnsureCapacity((emxArray__common *)r15, i10, (int32_T)sizeof(real_T));
  r1 = r14->size[0] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r15->data[i10] = sindex + r14->data[i10];
  }

  emxFree_real_T(&r14);
  emxInit_int32_T(&r16, 2);

  /* 'buildmanouvers:22' c = tan(thetadiffr); */
  c = tan(thetadiffr);

  /* 'buildmanouvers:24' npaths = length(qflist); */
  /* preallocate manouver x y corrdinates */
  /* 'buildmanouvers:27' mxi = zeros(length(mank),length(qflist)); */
  i10 = r16->size[0] * r16->size[1];
  r16->size[0] = 1;
  r16->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r16, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r16->data[r16->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r17, 2);
  i10 = r17->size[0] * r17->size[1];
  r17->size[0] = 1;
  r17->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r17, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r17->data[r17->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&b_kk, 1);
  r1 = r16->size[1];
  i10 = b_kk->size[0];
  b_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)b_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r16);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    b_kk->data[i10] = kk->data[r17->data[i10] - 1];
  }

  emxFree_int32_T(&r17);
  emxInit_int32_T(&r18, 2);
  r1 = b_kk->size[0];
  i10 = mxi->size[0] * mxi->size[1];
  mxi->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)mxi, i10, (int32_T)sizeof(real_T));
  i10 = r18->size[0] * r18->size[1];
  r18->size[0] = 1;
  r18->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r18, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&b_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r18->data[r18->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r19, 2);
  i10 = r19->size[0] * r19->size[1];
  r19->size[0] = 1;
  r19->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r19, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r19->data[r19->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&c_kk, 1);
  r1 = r18->size[1];
  i10 = c_kk->size[0];
  c_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)c_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r18);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    c_kk->data[i10] = kk->data[r19->data[i10] - 1];
  }

  emxFree_int32_T(&r19);
  emxInit_int32_T(&r20, 2);
  r1 = qflist->size[1];
  i10 = mxi->size[0] * mxi->size[1];
  mxi->size[1] = r1;
  emxEnsureCapacity((emxArray__common *)mxi, i10, (int32_T)sizeof(real_T));
  i10 = r20->size[0] * r20->size[1];
  r20->size[0] = 1;
  r20->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r20, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&c_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r20->data[r20->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r21, 2);
  i10 = r21->size[0] * r21->size[1];
  r21->size[0] = 1;
  r21->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r21, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r21->data[r21->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&d_kk, 1);
  r1 = r20->size[1];
  i10 = d_kk->size[0];
  d_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)d_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r20);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    d_kk->data[i10] = kk->data[r21->data[i10] - 1];
  }

  emxFree_int32_T(&r21);
  r1 = d_kk->size[0] * qflist->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    mxi->data[i10] = 0.0;
  }

  emxFree_real_T(&d_kk);
  emxInit_int32_T(&r22, 2);

  /* 'buildmanouvers:28' myi = zeros(length(mank),length(qflist)); */
  i10 = r22->size[0] * r22->size[1];
  r22->size[0] = 1;
  r22->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r22, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r22->data[r22->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r23, 2);
  i10 = r23->size[0] * r23->size[1];
  r23->size[0] = 1;
  r23->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r23, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r23->data[r23->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&e_kk, 1);
  r1 = r22->size[1];
  i10 = e_kk->size[0];
  e_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)e_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r22);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    e_kk->data[i10] = kk->data[r23->data[i10] - 1];
  }

  emxFree_int32_T(&r23);
  emxInit_int32_T(&r24, 2);
  r1 = e_kk->size[0];
  i10 = myi->size[0] * myi->size[1];
  myi->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)myi, i10, (int32_T)sizeof(real_T));
  i10 = r24->size[0] * r24->size[1];
  r24->size[0] = 1;
  r24->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r24, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&e_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r24->data[r24->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r25, 2);
  i10 = r25->size[0] * r25->size[1];
  r25->size[0] = 1;
  r25->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r25, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r25->data[r25->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&f_kk, 1);
  r1 = r24->size[1];
  i10 = f_kk->size[0];
  f_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)f_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r24);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    f_kk->data[i10] = kk->data[r25->data[i10] - 1];
  }

  emxFree_int32_T(&r25);
  emxInit_int32_T(&r26, 2);
  r1 = qflist->size[1];
  i10 = myi->size[0] * myi->size[1];
  myi->size[1] = r1;
  emxEnsureCapacity((emxArray__common *)myi, i10, (int32_T)sizeof(real_T));
  i10 = r26->size[0] * r26->size[1];
  r26->size[0] = 1;
  r26->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r26, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&f_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r26->data[r26->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r27, 2);
  i10 = r27->size[0] * r27->size[1];
  r27->size[0] = 1;
  r27->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r27, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r27->data[r27->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&g_kk, 1);
  r1 = r26->size[1];
  i10 = g_kk->size[0];
  g_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)g_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r26);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    g_kk->data[i10] = kk->data[r27->data[i10] - 1];
  }

  emxFree_int32_T(&r27);
  r1 = g_kk->size[0] * qflist->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    myi->data[i10] = 0.0;
  }

  emxFree_real_T(&g_kk);
  emxInit_int32_T(&r28, 2);

  /* 'buildmanouvers:29' pathki = zeros(length(mank),length(qflist)); */
  i10 = r28->size[0] * r28->size[1];
  r28->size[0] = 1;
  r28->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r28, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r28->data[r28->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r29, 2);
  i10 = r29->size[0] * r29->size[1];
  r29->size[0] = 1;
  r29->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r29, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r29->data[r29->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&h_kk, 1);
  r1 = r28->size[1];
  i10 = h_kk->size[0];
  h_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)h_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r28);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    h_kk->data[i10] = kk->data[r29->data[i10] - 1];
  }

  emxFree_int32_T(&r29);
  emxInit_int32_T(&r30, 2);
  r1 = h_kk->size[0];
  i10 = pathki->size[0] * pathki->size[1];
  pathki->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)pathki, i10, (int32_T)sizeof(real_T));
  i10 = r30->size[0] * r30->size[1];
  r30->size[0] = 1;
  r30->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r30, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&h_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r30->data[r30->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r31, 2);
  i10 = r31->size[0] * r31->size[1];
  r31->size[0] = 1;
  r31->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r31, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r31->data[r31->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&i_kk, 1);
  r1 = r30->size[1];
  i10 = i_kk->size[0];
  i_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)i_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r30);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    i_kk->data[i10] = kk->data[r31->data[i10] - 1];
  }

  emxFree_int32_T(&r31);
  emxInit_int32_T(&r32, 2);
  r1 = qflist->size[1];
  i10 = pathki->size[0] * pathki->size[1];
  pathki->size[1] = r1;
  emxEnsureCapacity((emxArray__common *)pathki, i10, (int32_T)sizeof(real_T));
  i10 = r32->size[0] * r32->size[1];
  r32->size[0] = 1;
  r32->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r32, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&i_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r32->data[r32->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r33, 2);
  i10 = r33->size[0] * r33->size[1];
  r33->size[0] = 1;
  r33->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r33, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r33->data[r33->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&j_kk, 1);
  r1 = r32->size[1];
  i10 = j_kk->size[0];
  j_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)j_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r32);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    j_kk->data[i10] = kk->data[r33->data[i10] - 1];
  }

  emxFree_int32_T(&r33);
  r1 = j_kk->size[0] * qflist->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    pathki->data[i10] = 0.0;
  }

  emxFree_real_T(&j_kk);
  emxInit_int32_T(&r34, 2);

  /* 'buildmanouvers:30' pathqi = zeros(length(mank),length(qflist)); */
  i10 = r34->size[0] * r34->size[1];
  r34->size[0] = 1;
  r34->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r34, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r34->data[r34->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r35, 2);
  i10 = r35->size[0] * r35->size[1];
  r35->size[0] = 1;
  r35->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r35, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r35->data[r35->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&k_kk, 1);
  r1 = r34->size[1];
  i10 = k_kk->size[0];
  k_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)k_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r34);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    k_kk->data[i10] = kk->data[r35->data[i10] - 1];
  }

  emxFree_int32_T(&r35);
  emxInit_int32_T(&r36, 2);
  r1 = k_kk->size[0];
  i10 = pathqi->size[0] * pathqi->size[1];
  pathqi->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)pathqi, i10, (int32_T)sizeof(real_T));
  i10 = r36->size[0] * r36->size[1];
  r36->size[0] = 1;
  r36->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r36, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&k_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r36->data[r36->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r37, 2);
  i10 = r37->size[0] * r37->size[1];
  r37->size[0] = 1;
  r37->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r37, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r37->data[r37->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&l_kk, 1);
  r1 = r36->size[1];
  i10 = l_kk->size[0];
  l_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)l_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r36);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    l_kk->data[i10] = kk->data[r37->data[i10] - 1];
  }

  emxFree_int32_T(&r37);
  emxInit_int32_T(&r38, 2);
  r1 = qflist->size[1];
  i10 = pathqi->size[0] * pathqi->size[1];
  pathqi->size[1] = r1;
  emxEnsureCapacity((emxArray__common *)pathqi, i10, (int32_T)sizeof(real_T));
  i10 = r38->size[0] * r38->size[1];
  r38->size[0] = 1;
  r38->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r38, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&l_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r38->data[r38->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r39, 2);
  i10 = r39->size[0] * r39->size[1];
  r39->size[0] = 1;
  r39->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r39, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r39->data[r39->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&m_kk, 1);
  r1 = r38->size[1];
  i10 = m_kk->size[0];
  m_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)m_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r38);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    m_kk->data[i10] = kk->data[r39->data[i10] - 1];
  }

  emxFree_int32_T(&r39);
  r1 = m_kk->size[0] * qflist->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    pathqi->data[i10] = 0.0;
  }

  emxFree_real_T(&m_kk);
  emxInit_int32_T(&r40, 2);

  /* 'buildmanouvers:31' dthetai = zeros(length(mank),length(qflist)); */
  i10 = r40->size[0] * r40->size[1];
  r40->size[0] = 1;
  r40->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r40, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r40->data[r40->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r41, 2);
  i10 = r41->size[0] * r41->size[1];
  r41->size[0] = 1;
  r41->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r41, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r41->data[r41->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&n_kk, 1);
  r1 = r40->size[1];
  i10 = n_kk->size[0];
  n_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)n_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r40);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    n_kk->data[i10] = kk->data[r41->data[i10] - 1];
  }

  emxFree_int32_T(&r41);
  emxInit_int32_T(&r42, 2);
  r1 = n_kk->size[0];
  i10 = dthetai->size[0] * dthetai->size[1];
  dthetai->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)dthetai, i10, (int32_T)sizeof(real_T));
  i10 = r42->size[0] * r42->size[1];
  r42->size[0] = 1;
  r42->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r42, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&n_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r42->data[r42->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r43, 2);
  i10 = r43->size[0] * r43->size[1];
  r43->size[0] = 1;
  r43->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r43, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r43->data[r43->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&o_kk, 1);
  r1 = r42->size[1];
  i10 = o_kk->size[0];
  o_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)o_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r42);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    o_kk->data[i10] = kk->data[r43->data[i10] - 1];
  }

  emxFree_int32_T(&r43);
  emxInit_int32_T(&r44, 2);
  r1 = qflist->size[1];
  i10 = dthetai->size[0] * dthetai->size[1];
  dthetai->size[1] = r1;
  emxEnsureCapacity((emxArray__common *)dthetai, i10, (int32_T)sizeof(real_T));
  i10 = r44->size[0] * r44->size[1];
  r44->size[0] = 1;
  r44->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r44, i10, (int32_T)sizeof(int32_T));
  emxFree_real_T(&o_kk);
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r44->data[r44->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_int32_T(&r45, 2);
  i10 = r45->size[0] * r45->size[1];
  r45->size[0] = 1;
  r45->size[1] = r15->size[1];
  emxEnsureCapacity((emxArray__common *)r45, i10, (int32_T)sizeof(int32_T));
  r1 = r15->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    r45->data[r45->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
  }

  emxInit_real_T(&p_kk, 1);
  r1 = r44->size[1];
  i10 = p_kk->size[0];
  p_kk->size[0] = r1;
  emxEnsureCapacity((emxArray__common *)p_kk, i10, (int32_T)sizeof(real_T));
  emxFree_int32_T(&r44);
  r1--;
  for (i10 = 0; i10 <= r1; i10++) {
    p_kk->data[i10] = kk->data[r45->data[i10] - 1];
  }

  emxFree_int32_T(&r45);
  r1 = p_kk->size[0] * qflist->size[1] - 1;
  for (i10 = 0; i10 <= r1; i10++) {
    dthetai->data[i10] = 0.0;
  }

  emxFree_real_T(&p_kk);
  b_emxInit_real_T(&removeindex, 2);

  /* 'buildmanouvers:33' removeindex = []; */
  i10 = removeindex->size[0] * removeindex->size[1];
  removeindex->size[0] = 1;
  removeindex->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)removeindex, i10, (int32_T)sizeof(real_T));

  /* 'buildmanouvers:35' for index = 1:npaths */
  b_index = 0;
  emxInit_real_T(&q, 1);
  emxInit_real_T(&dq, 1);
  emxInit_real_T(&S, 1);
  emxInit_real_T(&Q, 1);
  b_emxInit_real_T(&theta, 2);
  b_emxInit_real_T(&my, 2);
  emxInit_real_T(&b, 1);
  emxInit_boolean_T(&x, 1);
  emxInit_real_T(&r46, 1);
  emxInit_real_T(&b_ss, 1);
  emxInit_real_T(&b_Q, 1);
  emxInit_real_T(&c_ss, 1);
  emxInit_real_T(&c_Q, 1);
  emxInit_real_T(&d_ss, 1);
  emxInit_real_T(&r47, 1);
  emxInit_real_T(&r48, 1);
  emxInit_real_T(&e_ss, 1);
  emxInit_real_T(&f_ss, 1);
  emxInit_real_T(&g_ss, 1);
  emxInit_int32_T(&r49, 2);
  emxInit_int32_T(&r50, 2);
  emxInit_int32_T(&r51, 2);
  emxInit_int32_T(&r52, 2);
  emxInit_int32_T(&r53, 2);
  emxInit_real_T(&q_kk, 1);
  emxInit_int32_T(&r54, 2);
  emxInit_int32_T(&r55, 2);
  emxInit_int32_T(&r56, 2);
  emxInit_int32_T(&r57, 2);
  emxInit_int32_T(&r58, 2);
  emxInit_int32_T(&r59, 2);
  emxInit_int32_T(&r60, 2);
  emxInit_int32_T(&r61, 2);
  emxInit_int32_T(&r62, 2);
  emxInit_int32_T(&r63, 2);
  emxInit_int32_T(&r64, 2);
  emxInit_int32_T(&r65, 2);
  emxInit_int32_T(&r66, 2);
  b_emxInit_int32_T(&r67, 1);
  emxInit_int32_T(&r68, 2);
  b_emxInit_int32_T(&r69, 1);
  emxInit_int32_T(&r70, 2);
  b_emxInit_real_T(&b_removeindex, 2);
  b_emxInit_real_T(&c_removeindex, 2);
  while (b_index <= qflist->size[1] - 1) {
    /* 'buildmanouvers:36' qf = qflist(index); */
    /* 'buildmanouvers:37' mcoef = [(sf-sii).^3 (sf-sii).^2;3.*(sf-sii).^2 2.*(sf-sii)]... */
    /* 'buildmanouvers:38'     \[qf - (qi + c.*(sf-sii));-c]; */
    A[0] = rt_powd_snf(sf - scp, 3.0);
    A[2] = rt_powd_snf(sf - scp, 2.0);
    A[1] = 3.0 * rt_powd_snf(sf - scp, 2.0);
    A[3] = 2.0 * (sf - scp);
    B[0] = qflist->data[(int32_T)(1.0 + (real_T)b_index) - 1] - (cpdistance + c *
      (sf - scp));
    B[1] = -c;
    if (fabs(A[1]) > fabs(A[0])) {
      r1 = 1;
      r2 = 0;
    } else {
      r1 = 0;
      r2 = 1;
    }

    nsmallsegments = A[r2] / A[r1];
    a22 = A[2 + r2] - nsmallsegments * A[2 + r1];
    mcoef_idx_1 = (B[r2] - B[r1] * nsmallsegments) / a22;
    nsmallsegments = (B[r1] - mcoef_idx_1 * A[2 + r1]) / A[r1];

    /* 'buildmanouvers:39' a = mcoef(1); */
    /* 'buildmanouvers:40' b = mcoef(2); */
    /* 'buildmanouvers:42' q = a.*(mans-sii).^3+b.*(mans-sii).^2+c.*(mans-sii)+qi; */
    i10 = r49->size[0] * r49->size[1];
    r49->size[0] = 1;
    r49->size[1] = r12->size[1];
    emxEnsureCapacity((emxArray__common *)r49, i10, (int32_T)sizeof(int32_T));
    r1 = r12->size[1] - 1;
    for (i10 = 0; i10 <= r1; i10++) {
      r49->data[r49->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
    }

    i10 = g_ss->size[0];
    g_ss->size[0] = r49->size[1];
    emxEnsureCapacity((emxArray__common *)g_ss, i10, (int32_T)sizeof(real_T));
    r1 = r49->size[1] - 1;
    for (i10 = 0; i10 <= r1; i10++) {
      g_ss->data[i10] = ss->data[(int32_T)r12->data[i10] - 1] - scp;
    }

    b_power(g_ss, r46);
    i10 = r46->size[0];
    r46->size[0] = r46->size[0];
    emxEnsureCapacity((emxArray__common *)r46, i10, (int32_T)sizeof(real_T));
    r1 = r46->size[0] - 1;
    for (i10 = 0; i10 <= r1; i10++) {
      r46->data[i10] *= nsmallsegments;
    }

    i10 = r50->size[0] * r50->size[1];
    r50->size[0] = 1;
    r50->size[1] = r12->size[1];
    emxEnsureCapacity((emxArray__common *)r50, i10, (int32_T)sizeof(int32_T));
    r1 = r12->size[1] - 1;
    for (i10 = 0; i10 <= r1; i10++) {
      r50->data[r50->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
    }

    i10 = f_ss->size[0];
    f_ss->size[0] = r50->size[1];
    emxEnsureCapacity((emxArray__common *)f_ss, i10, (int32_T)sizeof(real_T));
    r1 = r50->size[1] - 1;
    for (i10 = 0; i10 <= r1; i10++) {
      f_ss->data[i10] = ss->data[(int32_T)r12->data[i10] - 1] - scp;
    }

    power(f_ss, b);
    i10 = b->size[0];
    b->size[0] = b->size[0];
    emxEnsureCapacity((emxArray__common *)b, i10, (int32_T)sizeof(real_T));
    r1 = b->size[0] - 1;
    for (i10 = 0; i10 <= r1; i10++) {
      b->data[i10] *= mcoef_idx_1;
    }

    i10 = r51->size[0] * r51->size[1];
    r51->size[0] = 1;
    r51->size[1] = r12->size[1];
    emxEnsureCapacity((emxArray__common *)r51, i10, (int32_T)sizeof(int32_T));
    r1 = r12->size[1] - 1;
    for (i10 = 0; i10 <= r1; i10++) {
      r51->data[r51->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
    }

    i10 = q->size[0];
    q->size[0] = r46->size[0];
    emxEnsureCapacity((emxArray__common *)q, i10, (int32_T)sizeof(real_T));
    r1 = r46->size[0] - 1;
    for (i10 = 0; i10 <= r1; i10++) {
      q->data[i10] = ((r46->data[i10] + b->data[i10]) + c * (ss->data[(int32_T)
        r12->data[i10] - 1] - scp)) + cpdistance;
    }

    /* check to see if lateral offset is greater than base frame curvature */
    /* 'buildmanouvers:45' invalid = false; */
    invalid = FALSE;

    /* 'buildmanouvers:46' for i = 1:length(mank) */
    r2 = 0;
    do {
      exitg3 = 0;
      i10 = r52->size[0] * r52->size[1];
      r52->size[0] = 1;
      r52->size[1] = r15->size[1];
      emxEnsureCapacity((emxArray__common *)r52, i10, (int32_T)sizeof(int32_T));
      r1 = r15->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r52->data[r52->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
      }

      i10 = r53->size[0] * r53->size[1];
      r53->size[0] = 1;
      r53->size[1] = r15->size[1];
      emxEnsureCapacity((emxArray__common *)r53, i10, (int32_T)sizeof(int32_T));
      r1 = r15->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r53->data[r53->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
      }

      r1 = r52->size[1];
      i10 = q_kk->size[0];
      q_kk->size[0] = r1;
      emxEnsureCapacity((emxArray__common *)q_kk, i10, (int32_T)sizeof(real_T));
      r1--;
      for (i10 = 0; i10 <= r1; i10++) {
        q_kk->data[i10] = kk->data[r53->data[i10] - 1];
      }

      if (r2 <= q_kk->size[0] - 1) {
        /* 'buildmanouvers:47' if 1/mank(i) == Inf */
        i10 = r54->size[0] * r54->size[1];
        r54->size[0] = 1;
        r54->size[1] = r15->size[1];
        emxEnsureCapacity((emxArray__common *)r54, i10, (int32_T)sizeof(int32_T));
        r1 = r15->size[1] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          r54->data[r54->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
        }

        guard1 = FALSE;
        if (1.0 / kk->data[(int32_T)r15->data[r2] - 1] == rtInf) {
          guard1 = TRUE;
        } else {
          /* 'buildmanouvers:50' if q(i)*(1./mank(i)) <= 0 */
          i10 = r55->size[0] * r55->size[1];
          r55->size[0] = 1;
          r55->size[1] = r15->size[1];
          emxEnsureCapacity((emxArray__common *)r55, i10, (int32_T)sizeof
                            (int32_T));
          r1 = r15->size[1] - 1;
          for (i10 = 0; i10 <= r1; i10++) {
            r55->data[r55->size[0] * i10] = (int32_T)r15->data[r15->size[0] *
              i10];
          }

          if (q->data[r2] * (1.0 / kk->data[(int32_T)r15->data[r2] - 1]) <= 0.0)
          {
            /* different directions so dont care. */
            guard1 = TRUE;
          } else {
            /* 'buildmanouvers:53' if q(i) > 0 */
            if (q->data[r2] > 0.0) {
              /* +ve curvature */
              /* 'buildmanouvers:54' invalid = q(i) > 1./mank(i); */
              i10 = r57->size[0] * r57->size[1];
              r57->size[0] = 1;
              r57->size[1] = r15->size[1];
              emxEnsureCapacity((emxArray__common *)r57, i10, (int32_T)sizeof
                                (int32_T));
              r1 = r15->size[1] - 1;
              for (i10 = 0; i10 <= r1; i10++) {
                r57->data[r57->size[0] * i10] = (int32_T)r15->data[r15->size[0] *
                  i10];
              }

              invalid = (q->data[r2] > 1.0 / kk->data[(int32_T)r15->data[r2] - 1]);

              /* 'buildmanouvers:55' if invalid */
              if (invalid) {
                exitg3 = 1;
              } else {
                /* 'buildmanouvers:57' else */
                guard1 = TRUE;
              }
            } else {
              /* 'buildmanouvers:61' if q(i) < 0 */
              if (q->data[r2] < 0.0) {
                /* -ve curvature */
                /* 'buildmanouvers:62' invalid = q(i) < 1./mank(i); */
                i10 = r56->size[0] * r56->size[1];
                r56->size[0] = 1;
                r56->size[1] = r15->size[1];
                emxEnsureCapacity((emxArray__common *)r56, i10, (int32_T)sizeof
                                  (int32_T));
                r1 = r15->size[1] - 1;
                for (i10 = 0; i10 <= r1; i10++) {
                  r56->data[r56->size[0] * i10] = (int32_T)r15->data[r15->size[0]
                    * i10];
                }

                invalid = (q->data[r2] < 1.0 / kk->data[(int32_T)r15->data[r2] -
                           1]);

                /* 'buildmanouvers:63' if invalid */
                if (invalid) {
                  exitg3 = 1;
                } else {
                  /* 'buildmanouvers:65' else */
                  guard1 = TRUE;
                }
              } else {
                guard1 = TRUE;
              }
            }
          }
        }

        if (guard1 == TRUE) {
          r2++;
        }
      } else {
        exitg3 = 1;
      }
    } while (exitg3 == 0U);

    /* if so, remove path from candidates */
    /* 'buildmanouvers:72' if invalid */
    if (invalid) {
      /* 'buildmanouvers:73' removeindex = [removeindex index]; */
      i10 = c_removeindex->size[0] * c_removeindex->size[1];
      c_removeindex->size[0] = 1;
      c_removeindex->size[1] = removeindex->size[1] + 1;
      emxEnsureCapacity((emxArray__common *)c_removeindex, i10, (int32_T)sizeof
                        (real_T));
      r1 = removeindex->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        c_removeindex->data[c_removeindex->size[0] * i10] = removeindex->
          data[removeindex->size[0] * i10];
      }

      c_removeindex->data[c_removeindex->size[0] * removeindex->size[1]] = 1.0 +
        (real_T)b_index;
      i10 = removeindex->size[0] * removeindex->size[1];
      removeindex->size[0] = 1;
      removeindex->size[1] = c_removeindex->size[1];
      emxEnsureCapacity((emxArray__common *)removeindex, i10, (int32_T)sizeof
                        (real_T));
      r1 = c_removeindex->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        removeindex->data[removeindex->size[0] * i10] = c_removeindex->
          data[c_removeindex->size[0] * i10];
      }
    } else {
      /* 'buildmanouvers:77' pathqi(:,index) = q; */
      r1 = q->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        pathqi->data[i10 + pathqi->size[0] * ((int32_T)(1.0 + (real_T)b_index) -
          1)] = q->data[i10];
      }

      /* 'buildmanouvers:79' dq = 3*a*((mans-sii).^2)+2*b*(mans-sii)+c; */
      y = 3.0 * nsmallsegments;
      i10 = r58->size[0] * r58->size[1];
      r58->size[0] = 1;
      r58->size[1] = r12->size[1];
      emxEnsureCapacity((emxArray__common *)r58, i10, (int32_T)sizeof(int32_T));
      r1 = r12->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r58->data[r58->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
      }

      i10 = e_ss->size[0];
      e_ss->size[0] = r58->size[1];
      emxEnsureCapacity((emxArray__common *)e_ss, i10, (int32_T)sizeof(real_T));
      r1 = r58->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        e_ss->data[i10] = ss->data[(int32_T)r12->data[i10] - 1] - scp;
      }

      power(e_ss, b);
      a22 = 2.0 * mcoef_idx_1;
      i10 = r59->size[0] * r59->size[1];
      r59->size[0] = 1;
      r59->size[1] = r12->size[1];
      emxEnsureCapacity((emxArray__common *)r59, i10, (int32_T)sizeof(int32_T));
      r1 = r12->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r59->data[r59->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
      }

      i10 = dq->size[0];
      dq->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)dq, i10, (int32_T)sizeof(real_T));
      r1 = b->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        dq->data[i10] = (y * b->data[i10] + a22 * (ss->data[(int32_T)r12->
          data[i10] - 1] - scp)) + c;
      }

      /* 'buildmanouvers:80' dq2 = 6*a.*(mans-sii)+2.*b; */
      y = 6.0 * nsmallsegments;

      /* 'buildmanouvers:82' S = sign(1-(q.*mank)); */
      i10 = r60->size[0] * r60->size[1];
      r60->size[0] = 1;
      r60->size[1] = r15->size[1];
      emxEnsureCapacity((emxArray__common *)r60, i10, (int32_T)sizeof(int32_T));
      r1 = r15->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r60->data[r60->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
      }

      i10 = b->size[0];
      b->size[0] = q->size[0];
      emxEnsureCapacity((emxArray__common *)b, i10, (int32_T)sizeof(real_T));
      r1 = q->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        b->data[i10] = 1.0 - q->data[i10] * kk->data[(int32_T)r15->data[i10] - 1];
      }

      i10 = S->size[0];
      S->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)S, i10, (int32_T)sizeof(real_T));
      r1 = b->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        S->data[i10] = b->data[i10];
      }

      for (r2 = 0; r2 <= b->size[0] - 1; r2++) {
        nsmallsegments = S->data[r2];
        if (nsmallsegments < 0.0) {
          nsmallsegments = -1.0;
        } else if (nsmallsegments > 0.0) {
          nsmallsegments = 1.0;
        } else {
          if (nsmallsegments == 0.0) {
            nsmallsegments = 0.0;
          }
        }

        S->data[r2] = nsmallsegments;
      }

      /* 'buildmanouvers:83' Q = sqrt(dq.^2+(1-q.*mank).^2); */
      power(dq, b);
      i10 = r61->size[0] * r61->size[1];
      r61->size[0] = 1;
      r61->size[1] = r15->size[1];
      emxEnsureCapacity((emxArray__common *)r61, i10, (int32_T)sizeof(int32_T));
      r1 = r15->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r61->data[r61->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
      }

      i10 = r48->size[0];
      r48->size[0] = q->size[0];
      emxEnsureCapacity((emxArray__common *)r48, i10, (int32_T)sizeof(real_T));
      r1 = q->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r48->data[i10] = 1.0 - q->data[i10] * kk->data[(int32_T)r15->data[i10] -
          1];
      }

      power(r48, r46);
      i10 = b->size[0];
      b->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)b, i10, (int32_T)sizeof(real_T));
      r1 = b->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        b->data[i10] += r46->data[i10];
      }

      i10 = Q->size[0];
      Q->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)Q, i10, (int32_T)sizeof(real_T));
      r1 = b->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        Q->data[i10] = b->data[i10];
      }

      for (r2 = 0; r2 <= b->size[0] - 1; r2++) {
        Q->data[(int32_T)(1.0 + (real_T)r2) - 1] = sqrt(Q->data[(int32_T)(1.0 +
          (real_T)r2) - 1]);
      }

      /* 'buildmanouvers:85' pathk = (S./Q).*(mank+(((1-q.*mank).*(dq2)+mank.*(dq.^2))./(Q.^2))); */
      power(dq, r46);
      power(Q, b);
      i10 = r62->size[0] * r62->size[1];
      r62->size[0] = 1;
      r62->size[1] = r15->size[1];
      emxEnsureCapacity((emxArray__common *)r62, i10, (int32_T)sizeof(int32_T));
      r1 = r15->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r62->data[r62->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
      }

      i10 = r63->size[0] * r63->size[1];
      r63->size[0] = 1;
      r63->size[1] = r12->size[1];
      emxEnsureCapacity((emxArray__common *)r63, i10, (int32_T)sizeof(int32_T));
      r1 = r12->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r63->data[r63->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
      }

      nsmallsegments = 2.0 * mcoef_idx_1;
      i10 = r64->size[0] * r64->size[1];
      r64->size[0] = 1;
      r64->size[1] = r15->size[1];
      emxEnsureCapacity((emxArray__common *)r64, i10, (int32_T)sizeof(int32_T));
      r1 = r15->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r64->data[r64->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
      }

      i10 = r47->size[0];
      r47->size[0] = q->size[0];
      emxEnsureCapacity((emxArray__common *)r47, i10, (int32_T)sizeof(real_T));
      r1 = q->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r47->data[i10] = (1.0 - q->data[i10] * kk->data[(int32_T)r15->data[i10]
                          - 1]) * (y * (ss->data[(int32_T)r12->data[i10] - 1] -
          scp) + nsmallsegments) + kk->data[(int32_T)r15->data[i10] - 1] *
          r46->data[i10];
      }

      rdivide(r47, b, r46);
      rdivide(S, Q, b);
      i10 = r65->size[0] * r65->size[1];
      r65->size[0] = 1;
      r65->size[1] = r15->size[1];
      emxEnsureCapacity((emxArray__common *)r65, i10, (int32_T)sizeof(int32_T));
      r1 = r15->size[1] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        r65->data[r65->size[0] * i10] = (int32_T)r15->data[r15->size[0] * i10];
      }

      i10 = S->size[0];
      S->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)S, i10, (int32_T)sizeof(real_T));
      r1 = b->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        S->data[i10] = b->data[i10] * (kk->data[(int32_T)r15->data[i10] - 1] +
          r46->data[i10]);
      }

      /* 'buildmanouvers:88' if sum(abs(1./pathk) < mincurverad) */
      i10 = b->size[0];
      b->size[0] = S->size[0];
      emxEnsureCapacity((emxArray__common *)b, i10, (int32_T)sizeof(real_T));
      r1 = S->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        b->data[i10] = 1.0 / S->data[i10];
      }

      unnamed_idx_0 = (uint32_T)b->size[0];
      i10 = dq->size[0];
      dq->size[0] = (int32_T)unnamed_idx_0;
      emxEnsureCapacity((emxArray__common *)dq, i10, (int32_T)sizeof(real_T));
      for (r2 = 0; r2 <= b->size[0] - 1; r2++) {
        dq->data[r2] = fabs(b->data[r2]);
      }

      i10 = x->size[0];
      x->size[0] = dq->size[0];
      emxEnsureCapacity((emxArray__common *)x, i10, (int32_T)sizeof(boolean_T));
      r1 = dq->size[0] - 1;
      for (i10 = 0; i10 <= r1; i10++) {
        x->data[i10] = (dq->data[i10] < mincurverad);
      }

      if (x->size[0] == 0) {
        y = 0.0;
      } else {
        r1 = x->size[0];
        y = (real_T)x->data[0];
        for (r2 = 2; r2 <= r1; r2++) {
          y += (real_T)x->data[r2 - 1];
        }
      }

      if (y != 0.0) {
        /* 'buildmanouvers:89' removeindex = [removeindex index] */
        i10 = b_removeindex->size[0] * b_removeindex->size[1];
        b_removeindex->size[0] = 1;
        b_removeindex->size[1] = removeindex->size[1] + 1;
        emxEnsureCapacity((emxArray__common *)b_removeindex, i10, (int32_T)
                          sizeof(real_T));
        r1 = removeindex->size[1] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          b_removeindex->data[b_removeindex->size[0] * i10] = removeindex->
            data[removeindex->size[0] * i10];
        }

        b_removeindex->data[b_removeindex->size[0] * removeindex->size[1]] = 1.0
          + (real_T)b_index;
        i10 = removeindex->size[0] * removeindex->size[1];
        removeindex->size[0] = 1;
        removeindex->size[1] = b_removeindex->size[1];
        emxEnsureCapacity((emxArray__common *)removeindex, i10, (int32_T)sizeof
                          (real_T));
        r1 = b_removeindex->size[1] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          removeindex->data[removeindex->size[0] * i10] = b_removeindex->
            data[b_removeindex->size[0] * i10];
        }
      } else {
        /* 'buildmanouvers:93' pathki(:,index) = pathk; */
        r1 = S->size[0] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          pathki->data[i10 + pathki->size[0] * ((int32_T)(1.0 + (real_T)b_index)
            - 1)] = S->data[i10];
        }

        /* 'buildmanouvers:96' dtheta = Q.*pathk; */
        i10 = S->size[0];
        S->size[0] = Q->size[0];
        emxEnsureCapacity((emxArray__common *)S, i10, (int32_T)sizeof(real_T));
        r1 = Q->size[0] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          S->data[i10] *= Q->data[i10];
        }

        /* 'buildmanouvers:97' dthetai(:,index) = dtheta; */
        r1 = S->size[0] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          dthetai->data[i10 + dthetai->size[0] * ((int32_T)(1.0 + (real_T)
            b_index) - 1)] = S->data[i10];
        }

        /* 'buildmanouvers:98' theta = cumtrapz(mans,dtheta) + thetadiffr + paththeta; */
        i10 = r66->size[0] * r66->size[1];
        r66->size[0] = 1;
        r66->size[1] = r12->size[1];
        emxEnsureCapacity((emxArray__common *)r66, i10, (int32_T)sizeof(int32_T));
        r1 = r12->size[1] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          r66->data[r66->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
        }

        i10 = d_ss->size[0];
        d_ss->size[0] = r66->size[1];
        emxEnsureCapacity((emxArray__common *)d_ss, i10, (int32_T)sizeof(real_T));
        r1 = r66->size[1] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          d_ss->data[i10] = ss->data[(int32_T)r12->data[i10] - 1];
        }

        cumtrapz(d_ss, S, theta);
        i10 = theta->size[0] * theta->size[1];
        theta->size[0] = theta->size[0];
        theta->size[1] = 1;
        emxEnsureCapacity((emxArray__common *)theta, i10, (int32_T)sizeof(real_T));
        r1 = theta->size[0];
        r2 = theta->size[1];
        r1 = r1 * r2 - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          theta->data[i10] = (theta->data[i10] + thetadiffr) + paththeta;
        }

        /* 'buildmanouvers:99' dy = Q.*sin(theta(:,1)); */
        i10 = theta->size[0];
        r1 = r46->size[0];
        r46->size[0] = i10;
        emxEnsureCapacity((emxArray__common *)r46, r1, (int32_T)sizeof(real_T));
        r1 = i10 - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          r46->data[i10] = theta->data[i10];
        }

        r2 = 0;
        do {
          exitg2 = 0;
          i10 = theta->size[0];
          r1 = r67->size[0];
          r67->size[0] = i10;
          emxEnsureCapacity((emxArray__common *)r67, r1, (int32_T)sizeof(int32_T));
          r1 = i10 - 1;
          for (i10 = 0; i10 <= r1; i10++) {
            r67->data[i10] = 1 + i10;
          }

          if (r2 <= r67->size[0] - 1) {
            r46->data[r2] = sin(r46->data[r2]);
            r2++;
          } else {
            exitg2 = 1;
          }
        } while (exitg2 == 0U);

        /* 'buildmanouvers:100' my = cumtrapz(mans,dy) + py; */
        i10 = r68->size[0] * r68->size[1];
        r68->size[0] = 1;
        r68->size[1] = r12->size[1];
        emxEnsureCapacity((emxArray__common *)r68, i10, (int32_T)sizeof(int32_T));
        r1 = r12->size[1] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          r68->data[r68->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
        }

        i10 = c_ss->size[0];
        c_ss->size[0] = r68->size[1];
        emxEnsureCapacity((emxArray__common *)c_ss, i10, (int32_T)sizeof(real_T));
        r1 = r68->size[1] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          c_ss->data[i10] = ss->data[(int32_T)r12->data[i10] - 1];
        }

        i10 = c_Q->size[0];
        c_Q->size[0] = Q->size[0];
        emxEnsureCapacity((emxArray__common *)c_Q, i10, (int32_T)sizeof(real_T));
        r1 = Q->size[0] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          c_Q->data[i10] = Q->data[i10] * r46->data[i10];
        }

        cumtrapz(c_ss, c_Q, my);
        i10 = my->size[0] * my->size[1];
        my->size[0] = my->size[0];
        my->size[1] = 1;
        emxEnsureCapacity((emxArray__common *)my, i10, (int32_T)sizeof(real_T));
        r1 = my->size[0];
        r2 = my->size[1];
        r1 = r1 * r2 - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          my->data[i10] += py;
        }

        /* 'buildmanouvers:101' myi(:,index) = my(:,1); */
        i10 = my->size[0] - 1;
        for (r1 = 0; r1 <= i10; r1++) {
          myi->data[r1 + myi->size[0] * ((int32_T)(1.0 + (real_T)b_index) - 1)] =
            my->data[r1];
        }

        /* 'buildmanouvers:102' dx = Q.*cos(theta(:,1)); */
        i10 = theta->size[0];
        r1 = r46->size[0];
        r46->size[0] = i10;
        emxEnsureCapacity((emxArray__common *)r46, r1, (int32_T)sizeof(real_T));
        r1 = i10 - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          r46->data[i10] = theta->data[i10];
        }

        r2 = 0;
        do {
          exitg1 = 0;
          i10 = theta->size[0];
          r1 = r69->size[0];
          r69->size[0] = i10;
          emxEnsureCapacity((emxArray__common *)r69, r1, (int32_T)sizeof(int32_T));
          r1 = i10 - 1;
          for (i10 = 0; i10 <= r1; i10++) {
            r69->data[i10] = 1 + i10;
          }

          if (r2 <= r69->size[0] - 1) {
            r46->data[r2] = cos(r46->data[r2]);
            r2++;
          } else {
            exitg1 = 1;
          }
        } while (exitg1 == 0U);

        /* 'buildmanouvers:103' mx = cumtrapz(mans,dx) + px; */
        i10 = r70->size[0] * r70->size[1];
        r70->size[0] = 1;
        r70->size[1] = r12->size[1];
        emxEnsureCapacity((emxArray__common *)r70, i10, (int32_T)sizeof(int32_T));
        r1 = r12->size[1] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          r70->data[r70->size[0] * i10] = (int32_T)r12->data[r12->size[0] * i10];
        }

        i10 = b_ss->size[0];
        b_ss->size[0] = r70->size[1];
        emxEnsureCapacity((emxArray__common *)b_ss, i10, (int32_T)sizeof(real_T));
        r1 = r70->size[1] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          b_ss->data[i10] = ss->data[(int32_T)r12->data[i10] - 1];
        }

        i10 = b_Q->size[0];
        b_Q->size[0] = Q->size[0];
        emxEnsureCapacity((emxArray__common *)b_Q, i10, (int32_T)sizeof(real_T));
        r1 = Q->size[0] - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          b_Q->data[i10] = Q->data[i10] * r46->data[i10];
        }

        cumtrapz(b_ss, b_Q, theta);
        i10 = theta->size[0] * theta->size[1];
        theta->size[0] = theta->size[0];
        theta->size[1] = 1;
        emxEnsureCapacity((emxArray__common *)theta, i10, (int32_T)sizeof(real_T));
        r1 = theta->size[0];
        r2 = theta->size[1];
        r1 = r1 * r2 - 1;
        for (i10 = 0; i10 <= r1; i10++) {
          theta->data[i10] += px;
        }

        /* 'buildmanouvers:104' mxi(:,index) = mx(:,1); */
        i10 = theta->size[0] - 1;
        for (r1 = 0; r1 <= i10; r1++) {
          mxi->data[r1 + mxi->size[0] * ((int32_T)(1.0 + (real_T)b_index) - 1)] =
            theta->data[r1];
        }
      }
    }

    b_index++;
  }

  emxFree_real_T(&c_removeindex);
  emxFree_real_T(&b_removeindex);
  emxFree_int32_T(&r70);
  emxFree_int32_T(&r69);
  emxFree_int32_T(&r68);
  emxFree_int32_T(&r67);
  emxFree_int32_T(&r66);
  emxFree_int32_T(&r65);
  emxFree_int32_T(&r64);
  emxFree_int32_T(&r63);
  emxFree_int32_T(&r62);
  emxFree_int32_T(&r61);
  emxFree_int32_T(&r60);
  emxFree_int32_T(&r59);
  emxFree_int32_T(&r58);
  emxFree_int32_T(&r57);
  emxFree_int32_T(&r56);
  emxFree_int32_T(&r55);
  emxFree_int32_T(&r54);
  emxFree_real_T(&q_kk);
  emxFree_int32_T(&r53);
  emxFree_int32_T(&r52);
  emxFree_int32_T(&r51);
  emxFree_int32_T(&r50);
  emxFree_int32_T(&r49);
  emxFree_real_T(&g_ss);
  emxFree_real_T(&f_ss);
  emxFree_real_T(&e_ss);
  emxFree_real_T(&r48);
  emxFree_real_T(&r47);
  emxFree_real_T(&d_ss);
  emxFree_real_T(&c_Q);
  emxFree_real_T(&c_ss);
  emxFree_real_T(&b_Q);
  emxFree_real_T(&b_ss);
  emxFree_real_T(&r46);
  emxFree_boolean_T(&x);
  emxFree_real_T(&b);
  emxFree_real_T(&r15);
  emxFree_real_T(&r12);
  emxFree_real_T(&my);
  emxFree_real_T(&theta);
  emxFree_real_T(&Q);
  emxFree_real_T(&S);
  emxFree_real_T(&dq);
  emxFree_real_T(&q);
  emxFree_real_T(&qflist);

  /* 'buildmanouvers:107' mxi(:,removeindex) = []; */
  eml_null_assignment(mxi, removeindex);

  /* 'buildmanouvers:108' myi(:,removeindex) = []; */
  eml_null_assignment(myi, removeindex);

  /* 'buildmanouvers:109' pathki(:,removeindex) = []; */
  eml_null_assignment(pathki, removeindex);

  /* 'buildmanouvers:110' pathqi(:,removeindex) = []; */
  eml_null_assignment(pathqi, removeindex);

  /* 'buildmanouvers:111' dthetai(:,removeindex) = []; */
  eml_null_assignment(dthetai, removeindex);
  emxFree_real_T(&removeindex);
}

/* End of code generation (buildmanouvers.cpp) */
