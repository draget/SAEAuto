/*
 * matlab_types.h
 *
 * Code generation for function 'arclengthcurve'
 *
 * C source code generated on: Mon Sep  1 19:20:44 2014
 *
 */

#ifndef __MATLAB_TYPES_H__
#define __MATLAB_TYPES_H__

/* Type Definitions */
#ifndef struct_emxArray__common
#define struct_emxArray__common
typedef struct emxArray__common
{
    void *data;
    int32_T *size;
    int32_T allocatedSize;
    int32_T numDimensions;
    boolean_T canFreeData;
} emxArray__common;
#endif
#ifndef struct_emxArray_boolean_T
#define struct_emxArray_boolean_T
typedef struct emxArray_boolean_T
{
    boolean_T *data;
    int32_T *size;
    int32_T allocatedSize;
    int32_T numDimensions;
    boolean_T canFreeData;
} emxArray_boolean_T;
#endif
#ifndef struct_emxArray_int32_T
#define struct_emxArray_int32_T
typedef struct emxArray_int32_T
{
    int32_T *data;
    int32_T *size;
    int32_T allocatedSize;
    int32_T numDimensions;
    boolean_T canFreeData;
} emxArray_int32_T;
#endif
#ifndef struct_emxArray_int32_T_7
#define struct_emxArray_int32_T_7
typedef struct emxArray_int32_T_7
{
    int32_T data[7];
    int32_T size[1];
} emxArray_int32_T_7;
#endif
#ifndef struct_emxArray_real_T
#define struct_emxArray_real_T
typedef struct emxArray_real_T
{
    real_T *data;
    int32_T *size;
    int32_T allocatedSize;
    int32_T numDimensions;
    boolean_T canFreeData;
} emxArray_real_T;
#endif
#ifndef struct_emxArray_real_T_1
#define struct_emxArray_real_T_1
typedef struct emxArray_real_T_1
{
    real_T data[1];
    int32_T size[1];
} emxArray_real_T_1;
#endif
#ifndef struct_emxArray_real_T_7
#define struct_emxArray_real_T_7
typedef struct emxArray_real_T_7
{
    real_T data[7];
    int32_T size[1];
} emxArray_real_T_7;
#endif

#endif
/* End of code generation (matlab_types.h) */
