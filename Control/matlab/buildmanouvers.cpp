/*
 * buildmanouvers.cpp
 *
 * Code generation for function 'buildmanouvers'
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
#include "cumtrapz.h"
#include "rdivide.h"
#include "power.h"
#include "mldivide.h"
#include "linspace.h"
#include "matlab_rtwutil.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
static void eml_null_assignment(emxArray_real_T *x, const emxArray_real_T *idx);

/* Function Definitions */

/*
 *
 */
static void eml_null_assignment(emxArray_real_T *x, const emxArray_real_T *idx)
{
  int32_T ncols;
  real_T d3;
  int32_T j;
  int32_T i27;
  int32_T i;
  emxArray_boolean_T *b;
  int32_T nb;
  int32_T k;
  int32_T i28;
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
      i27 = x->size[0];
      for (i = 0; i + 1 <= i27; i++) {
        x->data[i + x->size[0] * (j - 1)] = x->data[i + x->size[0] * j];
      }

      j++;
    }
  } else {
    b_emxInit_boolean_T(&b, 2);
    i27 = b->size[0] * b->size[1];
    b->size[0] = 1;
    emxEnsureCapacity((emxArray__common *)b, i27, (int32_T)sizeof(boolean_T));
    nb = x->size[1];
    i27 = b->size[0] * b->size[1];
    b->size[1] = nb;
    emxEnsureCapacity((emxArray__common *)b, i27, (int32_T)sizeof(boolean_T));
    nb = x->size[1] - 1;
    for (i27 = 0; i27 <= nb; i27++) {
      b->data[i27] = FALSE;
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
    i27 = x->size[1];
    for (k = 1; k <= i27; k++) {
      if ((k > nb) || (!b->data[k - 1])) {
        i28 = x->size[0];
        for (i = 0; i + 1 <= i28; i++) {
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
  i27 = b_x->size[0] * b_x->size[1];
  b_x->size[0] = i;
  b_x->size[1] = ncols;
  emxEnsureCapacity((emxArray__common *)b_x, i27, (int32_T)sizeof(real_T));
  nb = ncols - 1;
  for (i27 = 0; i27 <= nb; i27++) {
    j = i - 1;
    for (i28 = 0; i28 <= j; i28++) {
      b_x->data[i28 + b_x->size[0] * i27] = x->data[i28 + x->size[0] * i27];
    }
  }

  i27 = x->size[0] * x->size[1];
  x->size[0] = b_x->size[0];
  x->size[1] = b_x->size[1];
  emxEnsureCapacity((emxArray__common *)x, i27, (int32_T)sizeof(real_T));
  nb = b_x->size[1] - 1;
  for (i27 = 0; i27 <= nb; i27++) {
    j = b_x->size[0] - 1;
    for (i28 = 0; i28 <= j; i28++) {
      x->data[i28 + x->size[0] * i27] = b_x->data[i28 + b_x->size[0] * i27];
    }
  }

  emxFree_real_T(&b_x);
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
  real_T sf;
  real_T thetadiffr;
  real_T endindex;
  int32_T i11;
  int32_T i12;
  emxArray_int32_T *r11;
  int32_T i13;
  int32_T loop_ub;
  emxArray_int32_T *r12;
  int32_T vlen;
  emxArray_int32_T *r13;
  int32_T i14;
  emxArray_int32_T *r14;
  emxArray_real_T *b_kk;
  real_T c;
  emxArray_real_T *c_kk;
  emxArray_real_T *d_kk;
  emxArray_real_T *e_kk;
  emxArray_real_T *f_kk;
  emxArray_real_T *g_kk;
  emxArray_real_T *h_kk;
  emxArray_real_T *i_kk;
  emxArray_real_T *j_kk;
  emxArray_real_T *k_kk;
  emxArray_real_T *l_kk;
  emxArray_real_T *m_kk;
  emxArray_real_T *n_kk;
  emxArray_real_T *o_kk;
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
  emxArray_real_T *r15;
  emxArray_real_T *b_ss;
  emxArray_real_T *b_Q;
  emxArray_real_T *c_ss;
  emxArray_real_T *c_Q;
  emxArray_real_T *d_ss;
  emxArray_real_T *r16;
  emxArray_real_T *r17;
  emxArray_real_T *e_ss;
  emxArray_real_T *f_ss;
  emxArray_real_T *g_ss;
  emxArray_real_T *q_kk;
  emxArray_int32_T *r18;
  emxArray_int32_T *r19;
  emxArray_real_T *b_removeindex;
  emxArray_real_T *c_removeindex;
  real_T dv0[4];
  real_T b_qflist[2];
  real_T mcoef[2];
  boolean_T invalid;
  int32_T exitg3;
  boolean_T guard1 = FALSE;
  real_T y;
  int32_T k;
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
  /* qflist = qi + linspace(maxlatoffset,-maxlatoffset,ceil(2*maxlatoffset/mangran) + 1); %final offsets */
  /* final offsets */
  /* 'buildmanouvers:11' qflist = linspace(maxlatoffset,-maxlatoffset,ceil(2*maxlatoffset/mangran) + 1) %final offsets */
  linspace(maxlatoffset, -maxlatoffset, ceil(2.0 * maxlatoffset / mangran) + 1.0,
           qflist);

  /* 'buildmanouvers:12' sii = scp; */
  /* base manouver off point on base curve */
  /* 'buildmanouvers:13' sf = sii + manlength; */
  sf = scp + manlength;

  /* manouver  length */
  /* 'buildmanouvers:14' thetadiffr = thetadiff/360*(2*pi); */
  thetadiffr = thetadiff / 360.0 * 6.2831853071795862;

  /* heading angle, path angle difference */
  /* extract a list of arc lengths and baseframe curvature for the lenght of the manouver */
  /* 'buildmanouvers:18' smallsegmentlength = ss(2); */
  /* 'buildmanouvers:19' nsmallsegments = ceil(manlength/smallsegmentlength); */
  /* 'buildmanouvers:20' endindex = sindex+nsmallsegments; */
  endindex = sindex + ceil(manlength / ss->data[1]);

  /* 'buildmanouvers:21' if endindex > length(ss) */
  if (endindex > (real_T)ss->size[0]) {
    /* 'buildmanouvers:22' endindex = length(ss) */
    endindex = (real_T)ss->size[0];
  }

  /* 'buildmanouvers:24' mans = ss(sindex:endindex); */
  if (sindex > endindex) {
    i11 = 0;
    i12 = 0;
  } else {
    i11 = (int32_T)sindex - 1;
    i12 = (int32_T)endindex;
  }

  b_emxInit_int32_T(&r11, 1);
  i13 = r11->size[0];
  r11->size[0] = i12 - i11;
  emxEnsureCapacity((emxArray__common *)r11, i13, (int32_T)sizeof(int32_T));
  loop_ub = (i12 - i11) - 1;
  for (i13 = 0; i13 <= loop_ub; i13++) {
    r11->data[i13] = (i11 + i13) + 1;
  }

  emxInit_int32_T(&r12, 2);
  i13 = r12->size[0] * r12->size[1];
  r12->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)r12, i13, (int32_T)sizeof(int32_T));
  vlen = r11->size[0];
  i13 = r12->size[0] * r12->size[1];
  r12->size[1] = vlen;
  emxEnsureCapacity((emxArray__common *)r12, i13, (int32_T)sizeof(int32_T));
  loop_ub = r11->size[0] - 1;
  for (i13 = 0; i13 <= loop_ub; i13++) {
    r12->data[i13] = r11->data[i13];
  }

  emxFree_int32_T(&r11);
  i13 = mans->size[0];
  mans->size[0] = i12 - i11;
  emxEnsureCapacity((emxArray__common *)mans, i13, (int32_T)sizeof(real_T));
  loop_ub = (i12 - i11) - 1;
  for (i12 = 0; i12 <= loop_ub; i12++) {
    mans->data[i12] = ss->data[i11 + i12];
  }

  /* 'buildmanouvers:25' mank = kk(sindex:endindex); */
  if (sindex > endindex) {
    i12 = 0;
    i13 = 0;
  } else {
    i12 = (int32_T)sindex - 1;
    i13 = (int32_T)endindex;
  }

  b_emxInit_int32_T(&r13, 1);
  i14 = r13->size[0];
  r13->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)r13, i14, (int32_T)sizeof(int32_T));
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    r13->data[i14] = (i12 + i14) + 1;
  }

  emxInit_int32_T(&r14, 2);
  i14 = r14->size[0] * r14->size[1];
  r14->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)r14, i14, (int32_T)sizeof(int32_T));
  vlen = r13->size[0];
  i14 = r14->size[0] * r14->size[1];
  r14->size[1] = vlen;
  emxEnsureCapacity((emxArray__common *)r14, i14, (int32_T)sizeof(int32_T));
  loop_ub = r13->size[0] - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    r14->data[i14] = r13->data[i14];
  }

  emxFree_int32_T(&r13);
  emxInit_real_T(&b_kk, 1);

  /* 'buildmanouvers:27' c = tan(thetadiffr); */
  c = tan(thetadiffr);

  /* 'buildmanouvers:29' npaths = length(qflist); */
  /* preallocate manouver x y corrdinates */
  /* 'buildmanouvers:32' mxi = zeros(length(mank),length(qflist)); */
  i14 = b_kk->size[0];
  b_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)b_kk, i14, (int32_T)sizeof(real_T));
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    b_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&c_kk, 1);
  vlen = b_kk->size[0];
  i14 = mxi->size[0] * mxi->size[1];
  mxi->size[0] = vlen;
  emxEnsureCapacity((emxArray__common *)mxi, i14, (int32_T)sizeof(real_T));
  i14 = c_kk->size[0];
  c_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)c_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&b_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    c_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&d_kk, 1);
  vlen = qflist->size[1];
  i14 = mxi->size[0] * mxi->size[1];
  mxi->size[1] = vlen;
  emxEnsureCapacity((emxArray__common *)mxi, i14, (int32_T)sizeof(real_T));
  i14 = d_kk->size[0];
  d_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)d_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&c_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    d_kk->data[i14] = kk->data[i12 + i14];
  }

  loop_ub = d_kk->size[0] * qflist->size[1] - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    mxi->data[i14] = 0.0;
  }

  emxFree_real_T(&d_kk);
  emxInit_real_T(&e_kk, 1);

  /* 'buildmanouvers:33' myi = zeros(length(mank),length(qflist)); */
  i14 = e_kk->size[0];
  e_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)e_kk, i14, (int32_T)sizeof(real_T));
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    e_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&f_kk, 1);
  vlen = e_kk->size[0];
  i14 = myi->size[0] * myi->size[1];
  myi->size[0] = vlen;
  emxEnsureCapacity((emxArray__common *)myi, i14, (int32_T)sizeof(real_T));
  i14 = f_kk->size[0];
  f_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)f_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&e_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    f_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&g_kk, 1);
  vlen = qflist->size[1];
  i14 = myi->size[0] * myi->size[1];
  myi->size[1] = vlen;
  emxEnsureCapacity((emxArray__common *)myi, i14, (int32_T)sizeof(real_T));
  i14 = g_kk->size[0];
  g_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)g_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&f_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    g_kk->data[i14] = kk->data[i12 + i14];
  }

  loop_ub = g_kk->size[0] * qflist->size[1] - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    myi->data[i14] = 0.0;
  }

  emxFree_real_T(&g_kk);
  emxInit_real_T(&h_kk, 1);

  /* 'buildmanouvers:34' pathki = zeros(length(mank),length(qflist)); */
  i14 = h_kk->size[0];
  h_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)h_kk, i14, (int32_T)sizeof(real_T));
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    h_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&i_kk, 1);
  vlen = h_kk->size[0];
  i14 = pathki->size[0] * pathki->size[1];
  pathki->size[0] = vlen;
  emxEnsureCapacity((emxArray__common *)pathki, i14, (int32_T)sizeof(real_T));
  i14 = i_kk->size[0];
  i_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)i_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&h_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    i_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&j_kk, 1);
  vlen = qflist->size[1];
  i14 = pathki->size[0] * pathki->size[1];
  pathki->size[1] = vlen;
  emxEnsureCapacity((emxArray__common *)pathki, i14, (int32_T)sizeof(real_T));
  i14 = j_kk->size[0];
  j_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)j_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&i_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    j_kk->data[i14] = kk->data[i12 + i14];
  }

  loop_ub = j_kk->size[0] * qflist->size[1] - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    pathki->data[i14] = 0.0;
  }

  emxFree_real_T(&j_kk);
  emxInit_real_T(&k_kk, 1);

  /* 'buildmanouvers:35' pathqi = zeros(length(mank),length(qflist)); */
  i14 = k_kk->size[0];
  k_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)k_kk, i14, (int32_T)sizeof(real_T));
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    k_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&l_kk, 1);
  vlen = k_kk->size[0];
  i14 = pathqi->size[0] * pathqi->size[1];
  pathqi->size[0] = vlen;
  emxEnsureCapacity((emxArray__common *)pathqi, i14, (int32_T)sizeof(real_T));
  i14 = l_kk->size[0];
  l_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)l_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&k_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    l_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&m_kk, 1);
  vlen = qflist->size[1];
  i14 = pathqi->size[0] * pathqi->size[1];
  pathqi->size[1] = vlen;
  emxEnsureCapacity((emxArray__common *)pathqi, i14, (int32_T)sizeof(real_T));
  i14 = m_kk->size[0];
  m_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)m_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&l_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    m_kk->data[i14] = kk->data[i12 + i14];
  }

  loop_ub = m_kk->size[0] * qflist->size[1] - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    pathqi->data[i14] = 0.0;
  }

  emxFree_real_T(&m_kk);
  emxInit_real_T(&n_kk, 1);

  /* 'buildmanouvers:36' dthetai = zeros(length(mank),length(qflist)); */
  i14 = n_kk->size[0];
  n_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)n_kk, i14, (int32_T)sizeof(real_T));
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    n_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&o_kk, 1);
  vlen = n_kk->size[0];
  i14 = dthetai->size[0] * dthetai->size[1];
  dthetai->size[0] = vlen;
  emxEnsureCapacity((emxArray__common *)dthetai, i14, (int32_T)sizeof(real_T));
  i14 = o_kk->size[0];
  o_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)o_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&n_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    o_kk->data[i14] = kk->data[i12 + i14];
  }

  emxInit_real_T(&p_kk, 1);
  vlen = qflist->size[1];
  i14 = dthetai->size[0] * dthetai->size[1];
  dthetai->size[1] = vlen;
  emxEnsureCapacity((emxArray__common *)dthetai, i14, (int32_T)sizeof(real_T));
  i14 = p_kk->size[0];
  p_kk->size[0] = i13 - i12;
  emxEnsureCapacity((emxArray__common *)p_kk, i14, (int32_T)sizeof(real_T));
  emxFree_real_T(&o_kk);
  loop_ub = (i13 - i12) - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    p_kk->data[i14] = kk->data[i12 + i14];
  }

  loop_ub = p_kk->size[0] * qflist->size[1] - 1;
  for (i14 = 0; i14 <= loop_ub; i14++) {
    dthetai->data[i14] = 0.0;
  }

  emxFree_real_T(&p_kk);
  b_emxInit_real_T(&removeindex, 2);

  /* 'buildmanouvers:38' removeindex = []; */
  i14 = removeindex->size[0] * removeindex->size[1];
  removeindex->size[0] = 1;
  removeindex->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)removeindex, i14, (int32_T)sizeof(real_T));

  /* 'buildmanouvers:40' for index = 1:npaths */
  b_index = 0;
  emxInit_real_T(&q, 1);
  emxInit_real_T(&dq, 1);
  emxInit_real_T(&S, 1);
  emxInit_real_T(&Q, 1);
  b_emxInit_real_T(&theta, 2);
  b_emxInit_real_T(&my, 2);
  emxInit_real_T(&b, 1);
  emxInit_boolean_T(&x, 1);
  emxInit_real_T(&r15, 1);
  emxInit_real_T(&b_ss, 1);
  emxInit_real_T(&b_Q, 1);
  emxInit_real_T(&c_ss, 1);
  emxInit_real_T(&c_Q, 1);
  emxInit_real_T(&d_ss, 1);
  emxInit_real_T(&r16, 1);
  emxInit_real_T(&r17, 1);
  emxInit_real_T(&e_ss, 1);
  emxInit_real_T(&f_ss, 1);
  emxInit_real_T(&g_ss, 1);
  emxInit_real_T(&q_kk, 1);
  b_emxInit_int32_T(&r18, 1);
  b_emxInit_int32_T(&r19, 1);
  b_emxInit_real_T(&b_removeindex, 2);
  b_emxInit_real_T(&c_removeindex, 2);
  while (b_index <= qflist->size[1] - 1) {
    /* 'buildmanouvers:41' qf = qflist(index); */
    /* 'buildmanouvers:42' mcoef = [(sf-sii).^3 (sf-sii).^2;3.*(sf-sii).^2 2.*(sf-sii)]... */
    /* 'buildmanouvers:43'     \[qf - (qi + c.*(sf-sii));-c]; */
    dv0[0] = rt_powd_snf(sf - scp, 3.0);
    dv0[2] = rt_powd_snf(sf - scp, 2.0);
    dv0[1] = 3.0 * rt_powd_snf(sf - scp, 2.0);
    dv0[3] = 2.0 * (sf - scp);
    b_qflist[0] = qflist->data[(int32_T)(1.0 + (real_T)b_index) - 1] -
      (cpdistance + c * (sf - scp));
    b_qflist[1] = -c;
    mldivide(dv0, b_qflist, mcoef);

    /* 'buildmanouvers:44' a = mcoef(1); */
    /* 'buildmanouvers:45' b = mcoef(2); */
    /* 'buildmanouvers:47' q = a.*(mans-sii).^3+b.*(mans-sii).^2+c.*(mans-sii)+qi; */
    i14 = g_ss->size[0];
    g_ss->size[0] = r12->size[1];
    emxEnsureCapacity((emxArray__common *)g_ss, i14, (int32_T)sizeof(real_T));
    loop_ub = r12->size[1] - 1;
    for (i14 = 0; i14 <= loop_ub; i14++) {
      g_ss->data[i14] = ss->data[r12->data[i14] - 1] - scp;
    }

    b_power(g_ss, r15);
    i14 = r15->size[0];
    r15->size[0] = r15->size[0];
    emxEnsureCapacity((emxArray__common *)r15, i14, (int32_T)sizeof(real_T));
    loop_ub = r15->size[0] - 1;
    for (i14 = 0; i14 <= loop_ub; i14++) {
      r15->data[i14] *= mcoef[0];
    }

    i14 = f_ss->size[0];
    f_ss->size[0] = r12->size[1];
    emxEnsureCapacity((emxArray__common *)f_ss, i14, (int32_T)sizeof(real_T));
    loop_ub = r12->size[1] - 1;
    for (i14 = 0; i14 <= loop_ub; i14++) {
      f_ss->data[i14] = ss->data[r12->data[i14] - 1] - scp;
    }

    power(f_ss, b);
    i14 = b->size[0];
    b->size[0] = b->size[0];
    emxEnsureCapacity((emxArray__common *)b, i14, (int32_T)sizeof(real_T));
    loop_ub = b->size[0] - 1;
    for (i14 = 0; i14 <= loop_ub; i14++) {
      b->data[i14] *= mcoef[1];
    }

    i14 = q->size[0];
    q->size[0] = r15->size[0];
    emxEnsureCapacity((emxArray__common *)q, i14, (int32_T)sizeof(real_T));
    loop_ub = r15->size[0] - 1;
    for (i14 = 0; i14 <= loop_ub; i14++) {
      q->data[i14] = ((r15->data[i14] + b->data[i14]) + c * (ss->data[i11 + i14]
        - scp)) + cpdistance;
    }

    /* check to see if lateral offset is greater than base frame curvature */
    /* 'buildmanouvers:50' invalid = false; */
    invalid = FALSE;

    /* 'buildmanouvers:51' for i = 1:length(mank) */
    vlen = 0;
    do {
      exitg3 = 0;
      i14 = q_kk->size[0];
      q_kk->size[0] = i13 - i12;
      emxEnsureCapacity((emxArray__common *)q_kk, i14, (int32_T)sizeof(real_T));
      loop_ub = (i13 - i12) - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        q_kk->data[i14] = kk->data[i12 + i14];
      }

      if (vlen <= q_kk->size[0] - 1) {
        /* 'buildmanouvers:52' if 1/mank(i) == Inf */
        guard1 = FALSE;
        if ((1.0 / kk->data[r14->data[vlen] - 1] == rtInf) || (q->data[vlen] *
             (1.0 / kk->data[i12 + vlen]) <= 0.0)) {
          /* different directions so dont care. */
          guard1 = TRUE;
        } else {
          /* 'buildmanouvers:55' if q(i)*(1./mank(i)) <= 0 */
          /* 'buildmanouvers:58' if q(i) > 0 */
          if (q->data[vlen] > 0.0) {
            /* +ve curvature */
            /* 'buildmanouvers:59' invalid = q(i) > 1./mank(i); */
            invalid = (q->data[vlen] > 1.0 / kk->data[r14->data[vlen] - 1]);

            /* 'buildmanouvers:60' if invalid */
            if (invalid) {
              exitg3 = 1;
            } else {
              /* 'buildmanouvers:62' else */
              guard1 = TRUE;
            }
          } else {
            /* 'buildmanouvers:66' if q(i) < 0 */
            if (q->data[vlen] < 0.0) {
              /* -ve curvature */
              /* 'buildmanouvers:67' invalid = q(i) < 1./mank(i); */
              invalid = (q->data[vlen] < 1.0 / kk->data[r14->data[vlen] - 1]);

              /* 'buildmanouvers:68' if invalid */
              if (invalid) {
                exitg3 = 1;
              } else {
                /* 'buildmanouvers:70' else */
                guard1 = TRUE;
              }
            } else {
              guard1 = TRUE;
            }
          }
        }

        if (guard1 == TRUE) {
          vlen++;
        }
      } else {
        exitg3 = 1;
      }
    } while (exitg3 == 0U);

    /* if so, remove path from candidates */
    /* 'buildmanouvers:77' if invalid */
    if (invalid) {
      /* 'buildmanouvers:78' removeindex = [removeindex index]; */
      i14 = c_removeindex->size[0] * c_removeindex->size[1];
      c_removeindex->size[0] = 1;
      c_removeindex->size[1] = removeindex->size[1] + 1;
      emxEnsureCapacity((emxArray__common *)c_removeindex, i14, (int32_T)sizeof
                        (real_T));
      loop_ub = removeindex->size[1] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        c_removeindex->data[c_removeindex->size[0] * i14] = removeindex->
          data[removeindex->size[0] * i14];
      }

      c_removeindex->data[c_removeindex->size[0] * removeindex->size[1]] = 1.0 +
        (real_T)b_index;
      i14 = removeindex->size[0] * removeindex->size[1];
      removeindex->size[0] = 1;
      removeindex->size[1] = c_removeindex->size[1];
      emxEnsureCapacity((emxArray__common *)removeindex, i14, (int32_T)sizeof
                        (real_T));
      loop_ub = c_removeindex->size[1] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        removeindex->data[removeindex->size[0] * i14] = c_removeindex->
          data[c_removeindex->size[0] * i14];
      }
    } else {
      /* 'buildmanouvers:82' pathqi(:,index) = q; */
      loop_ub = q->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        pathqi->data[i14 + pathqi->size[0] * ((int32_T)(1.0 + (real_T)b_index) -
          1)] = q->data[i14];
      }

      /* 'buildmanouvers:84' dq = 3*a*((mans-sii).^2)+2*b*(mans-sii)+c; */
      y = 3.0 * mcoef[0];
      i14 = e_ss->size[0];
      e_ss->size[0] = r12->size[1];
      emxEnsureCapacity((emxArray__common *)e_ss, i14, (int32_T)sizeof(real_T));
      loop_ub = r12->size[1] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        e_ss->data[i14] = ss->data[r12->data[i14] - 1] - scp;
      }

      power(e_ss, b);
      endindex = 2.0 * mcoef[1];
      i14 = dq->size[0];
      dq->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)dq, i14, (int32_T)sizeof(real_T));
      loop_ub = b->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        dq->data[i14] = (y * b->data[i14] + endindex * (ss->data[r12->data[i14]
          - 1] - scp)) + c;
      }

      /* 'buildmanouvers:85' dq2 = 6*a.*(mans-sii)+2.*b; */
      y = 6.0 * mcoef[0];

      /* 'buildmanouvers:87' S = sign(1-(q.*mank)); */
      i14 = b->size[0];
      b->size[0] = q->size[0];
      emxEnsureCapacity((emxArray__common *)b, i14, (int32_T)sizeof(real_T));
      loop_ub = q->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        b->data[i14] = 1.0 - q->data[i14] * kk->data[r14->data[i14] - 1];
      }

      i14 = S->size[0];
      S->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)S, i14, (int32_T)sizeof(real_T));
      loop_ub = b->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        S->data[i14] = b->data[i14];
      }

      for (k = 0; k <= b->size[0] - 1; k++) {
        endindex = S->data[k];
        if (endindex < 0.0) {
          endindex = -1.0;
        } else if (endindex > 0.0) {
          endindex = 1.0;
        } else {
          if (endindex == 0.0) {
            endindex = 0.0;
          }
        }

        S->data[k] = endindex;
      }

      /* 'buildmanouvers:88' Q = sqrt(dq.^2+(1-q.*mank).^2); */
      power(dq, b);
      i14 = r17->size[0];
      r17->size[0] = q->size[0];
      emxEnsureCapacity((emxArray__common *)r17, i14, (int32_T)sizeof(real_T));
      loop_ub = q->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        r17->data[i14] = 1.0 - q->data[i14] * kk->data[r14->data[i14] - 1];
      }

      power(r17, r15);
      i14 = b->size[0];
      b->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)b, i14, (int32_T)sizeof(real_T));
      loop_ub = b->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        b->data[i14] += r15->data[i14];
      }

      i14 = Q->size[0];
      Q->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)Q, i14, (int32_T)sizeof(real_T));
      loop_ub = b->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        Q->data[i14] = b->data[i14];
      }

      for (k = 0; k <= b->size[0] - 1; k++) {
        Q->data[(int32_T)(1.0 + (real_T)k) - 1] = sqrt(Q->data[(int32_T)(1.0 +
          (real_T)k) - 1]);
      }

      /* 'buildmanouvers:90' pathk = (S./Q).*(mank+(((1-q.*mank).*(dq2)+mank.*(dq.^2))./(Q.^2))); */
      power(dq, r15);
      power(Q, b);
      endindex = 2.0 * mcoef[1];
      i14 = r16->size[0];
      r16->size[0] = q->size[0];
      emxEnsureCapacity((emxArray__common *)r16, i14, (int32_T)sizeof(real_T));
      loop_ub = q->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        r16->data[i14] = (1.0 - q->data[i14] * kk->data[r14->data[i14] - 1]) *
          (y * (ss->data[r12->data[i14] - 1] - scp) + endindex) + kk->data[i12 +
          i14] * r15->data[i14];
      }

      rdivide(r16, b, r15);
      rdivide(S, Q, b);
      i14 = S->size[0];
      S->size[0] = b->size[0];
      emxEnsureCapacity((emxArray__common *)S, i14, (int32_T)sizeof(real_T));
      loop_ub = b->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        S->data[i14] = b->data[i14] * (kk->data[r14->data[i14] - 1] + r15->
          data[i14]);
      }

      /* 'buildmanouvers:93' if sum(abs(1./pathk) < mincurverad) */
      i14 = b->size[0];
      b->size[0] = S->size[0];
      emxEnsureCapacity((emxArray__common *)b, i14, (int32_T)sizeof(real_T));
      loop_ub = S->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        b->data[i14] = 1.0 / S->data[i14];
      }

      unnamed_idx_0 = (uint32_T)b->size[0];
      i14 = dq->size[0];
      dq->size[0] = (int32_T)unnamed_idx_0;
      emxEnsureCapacity((emxArray__common *)dq, i14, (int32_T)sizeof(real_T));
      for (k = 0; k <= b->size[0] - 1; k++) {
        dq->data[k] = fabs(b->data[k]);
      }

      i14 = x->size[0];
      x->size[0] = dq->size[0];
      emxEnsureCapacity((emxArray__common *)x, i14, (int32_T)sizeof(boolean_T));
      loop_ub = dq->size[0] - 1;
      for (i14 = 0; i14 <= loop_ub; i14++) {
        x->data[i14] = (dq->data[i14] < mincurverad);
      }

      if (x->size[0] == 0) {
        y = 0.0;
      } else {
        vlen = x->size[0];
        y = (real_T)x->data[0];
        for (k = 2; k <= vlen; k++) {
          y += (real_T)x->data[k - 1];
        }
      }

      if (y != 0.0) {
        /* 'buildmanouvers:94' removeindex = [removeindex index]; */
        i14 = b_removeindex->size[0] * b_removeindex->size[1];
        b_removeindex->size[0] = 1;
        b_removeindex->size[1] = removeindex->size[1] + 1;
        emxEnsureCapacity((emxArray__common *)b_removeindex, i14, (int32_T)
                          sizeof(real_T));
        loop_ub = removeindex->size[1] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          b_removeindex->data[b_removeindex->size[0] * i14] = removeindex->
            data[removeindex->size[0] * i14];
        }

        b_removeindex->data[b_removeindex->size[0] * removeindex->size[1]] = 1.0
          + (real_T)b_index;
        i14 = removeindex->size[0] * removeindex->size[1];
        removeindex->size[0] = 1;
        removeindex->size[1] = b_removeindex->size[1];
        emxEnsureCapacity((emxArray__common *)removeindex, i14, (int32_T)sizeof
                          (real_T));
        loop_ub = b_removeindex->size[1] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          removeindex->data[removeindex->size[0] * i14] = b_removeindex->
            data[b_removeindex->size[0] * i14];
        }
      } else {
        /* 'buildmanouvers:98' pathki(:,index) = pathk; */
        loop_ub = S->size[0] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          pathki->data[i14 + pathki->size[0] * ((int32_T)(1.0 + (real_T)b_index)
            - 1)] = S->data[i14];
        }

        /* 'buildmanouvers:101' dtheta = Q.*pathk; */
        i14 = S->size[0];
        S->size[0] = Q->size[0];
        emxEnsureCapacity((emxArray__common *)S, i14, (int32_T)sizeof(real_T));
        loop_ub = Q->size[0] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          S->data[i14] *= Q->data[i14];
        }

        /* 'buildmanouvers:102' dthetai(:,index) = dtheta; */
        loop_ub = S->size[0] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          dthetai->data[i14 + dthetai->size[0] * ((int32_T)(1.0 + (real_T)
            b_index) - 1)] = S->data[i14];
        }

        /* 'buildmanouvers:103' theta = cumtrapz(mans,dtheta) + thetadiffr + paththeta; */
        i14 = d_ss->size[0];
        d_ss->size[0] = r12->size[1];
        emxEnsureCapacity((emxArray__common *)d_ss, i14, (int32_T)sizeof(real_T));
        loop_ub = r12->size[1] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          d_ss->data[i14] = ss->data[r12->data[i14] - 1];
        }

        cumtrapz(d_ss, S, theta);
        i14 = theta->size[0] * theta->size[1];
        theta->size[0] = theta->size[0];
        theta->size[1] = 1;
        emxEnsureCapacity((emxArray__common *)theta, i14, (int32_T)sizeof(real_T));
        vlen = theta->size[0];
        loop_ub = theta->size[1];
        loop_ub = vlen * loop_ub - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          theta->data[i14] = (theta->data[i14] + thetadiffr) + paththeta;
        }

        /* 'buildmanouvers:104' dy = Q.*sin(theta(:,1)); */
        i14 = theta->size[0];
        vlen = r15->size[0];
        r15->size[0] = i14;
        emxEnsureCapacity((emxArray__common *)r15, vlen, (int32_T)sizeof(real_T));
        loop_ub = i14 - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          r15->data[i14] = theta->data[i14];
        }

        k = 0;
        do {
          exitg2 = 0;
          i14 = theta->size[0];
          vlen = r18->size[0];
          r18->size[0] = i14;
          emxEnsureCapacity((emxArray__common *)r18, vlen, (int32_T)sizeof
                            (int32_T));
          loop_ub = i14 - 1;
          for (i14 = 0; i14 <= loop_ub; i14++) {
            r18->data[i14] = 1 + i14;
          }

          if (k <= r18->size[0] - 1) {
            r15->data[k] = sin(r15->data[k]);
            k++;
          } else {
            exitg2 = 1;
          }
        } while (exitg2 == 0U);

        /* 'buildmanouvers:105' my = cumtrapz(mans,dy) + py; */
        i14 = c_ss->size[0];
        c_ss->size[0] = r12->size[1];
        emxEnsureCapacity((emxArray__common *)c_ss, i14, (int32_T)sizeof(real_T));
        loop_ub = r12->size[1] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          c_ss->data[i14] = ss->data[r12->data[i14] - 1];
        }

        i14 = c_Q->size[0];
        c_Q->size[0] = Q->size[0];
        emxEnsureCapacity((emxArray__common *)c_Q, i14, (int32_T)sizeof(real_T));
        loop_ub = Q->size[0] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          c_Q->data[i14] = Q->data[i14] * r15->data[i14];
        }

        cumtrapz(c_ss, c_Q, my);
        i14 = my->size[0] * my->size[1];
        my->size[0] = my->size[0];
        my->size[1] = 1;
        emxEnsureCapacity((emxArray__common *)my, i14, (int32_T)sizeof(real_T));
        vlen = my->size[0];
        loop_ub = my->size[1];
        loop_ub = vlen * loop_ub - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          my->data[i14] += py;
        }

        /* 'buildmanouvers:106' myi(:,index) = my(:,1); */
        i14 = my->size[0] - 1;
        for (vlen = 0; vlen <= i14; vlen++) {
          myi->data[vlen + myi->size[0] * ((int32_T)(1.0 + (real_T)b_index) - 1)]
            = my->data[vlen];
        }

        /* 'buildmanouvers:107' dx = Q.*cos(theta(:,1)); */
        i14 = theta->size[0];
        vlen = r15->size[0];
        r15->size[0] = i14;
        emxEnsureCapacity((emxArray__common *)r15, vlen, (int32_T)sizeof(real_T));
        loop_ub = i14 - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          r15->data[i14] = theta->data[i14];
        }

        k = 0;
        do {
          exitg1 = 0;
          i14 = theta->size[0];
          vlen = r19->size[0];
          r19->size[0] = i14;
          emxEnsureCapacity((emxArray__common *)r19, vlen, (int32_T)sizeof
                            (int32_T));
          loop_ub = i14 - 1;
          for (i14 = 0; i14 <= loop_ub; i14++) {
            r19->data[i14] = 1 + i14;
          }

          if (k <= r19->size[0] - 1) {
            r15->data[k] = cos(r15->data[k]);
            k++;
          } else {
            exitg1 = 1;
          }
        } while (exitg1 == 0U);

        /* 'buildmanouvers:108' mx = cumtrapz(mans,dx) + px; */
        i14 = b_ss->size[0];
        b_ss->size[0] = r12->size[1];
        emxEnsureCapacity((emxArray__common *)b_ss, i14, (int32_T)sizeof(real_T));
        loop_ub = r12->size[1] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          b_ss->data[i14] = ss->data[r12->data[i14] - 1];
        }

        i14 = b_Q->size[0];
        b_Q->size[0] = Q->size[0];
        emxEnsureCapacity((emxArray__common *)b_Q, i14, (int32_T)sizeof(real_T));
        loop_ub = Q->size[0] - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          b_Q->data[i14] = Q->data[i14] * r15->data[i14];
        }

        cumtrapz(b_ss, b_Q, theta);
        i14 = theta->size[0] * theta->size[1];
        theta->size[0] = theta->size[0];
        theta->size[1] = 1;
        emxEnsureCapacity((emxArray__common *)theta, i14, (int32_T)sizeof(real_T));
        vlen = theta->size[0];
        loop_ub = theta->size[1];
        loop_ub = vlen * loop_ub - 1;
        for (i14 = 0; i14 <= loop_ub; i14++) {
          theta->data[i14] += px;
        }

        /* 'buildmanouvers:109' mxi(:,index) = mx(:,1); */
        i14 = theta->size[0] - 1;
        for (vlen = 0; vlen <= i14; vlen++) {
          mxi->data[vlen + mxi->size[0] * ((int32_T)(1.0 + (real_T)b_index) - 1)]
            = theta->data[vlen];
        }
      }
    }

    b_index++;
  }

  emxFree_real_T(&c_removeindex);
  emxFree_real_T(&b_removeindex);
  emxFree_int32_T(&r19);
  emxFree_int32_T(&r18);
  emxFree_real_T(&q_kk);
  emxFree_real_T(&g_ss);
  emxFree_real_T(&f_ss);
  emxFree_real_T(&e_ss);
  emxFree_real_T(&r17);
  emxFree_real_T(&r16);
  emxFree_real_T(&d_ss);
  emxFree_real_T(&c_Q);
  emxFree_real_T(&c_ss);
  emxFree_real_T(&b_Q);
  emxFree_real_T(&b_ss);
  emxFree_real_T(&r15);
  emxFree_boolean_T(&x);
  emxFree_real_T(&b);
  emxFree_int32_T(&r14);
  emxFree_int32_T(&r12);
  emxFree_real_T(&my);
  emxFree_real_T(&theta);
  emxFree_real_T(&Q);
  emxFree_real_T(&S);
  emxFree_real_T(&dq);
  emxFree_real_T(&q);
  emxFree_real_T(&qflist);

  /* 'buildmanouvers:112' mxi(:,removeindex) = []; */
  eml_null_assignment(mxi, removeindex);

  /* 'buildmanouvers:113' myi(:,removeindex) = []; */
  eml_null_assignment(myi, removeindex);

  /* 'buildmanouvers:114' pathki(:,removeindex) = []; */
  eml_null_assignment(pathki, removeindex);

  /* 'buildmanouvers:115' pathqi(:,removeindex) = []; */
  eml_null_assignment(pathqi, removeindex);

  /* 'buildmanouvers:116' dthetai(:,removeindex) = []; */
  eml_null_assignment(dthetai, removeindex);
  emxFree_real_T(&removeindex);
}

/* End of code generation (buildmanouvers.cpp) */
