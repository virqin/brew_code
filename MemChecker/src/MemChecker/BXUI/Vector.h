/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "IVector.h"
#include "Base.h"

typedef struct CVector
{
    CBase       base;
    void**      ppItems;
    uint32      nCount;
    uint32      cbCapacity;
    PFNNOTIFY   pfnFree;
    int         nGrowCount;
} CVector;

IVector*    CVector_New(void);
void        CVector_Ctor(CVector* pMe);
void        CVector_Dtor(CVector* pMe);
uint32      CVector_Release(IVector* po);
uint32      CVector_Size(IVector* po);
int         CVector_GetAt(IVector* po, uint32 nIndex, void** ppe);
int         CVector_Replace(IVector* po, uint32 nIndex, void* pe);
int         CVector_Insert(IVector* po, uint32 nIndex, void* pe);
int         CVector_DeleteAt(IVector* po, uint32 nIndex);
void        CVector_DeleteAll(IVector* po);
PFNNOTIFY   CVector_SetPfnFree(IVector* po, PFNNOTIFY pfnFree);
void        CVector_SetGrowCount(IVector* po, int n);
int         CVector_Reserve(IVector* po, uint32 n);

#endif // __VECTOR_H__
