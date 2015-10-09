/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "Vector.h"
#include "XUtil.h"

#define DEFAULT_GROW_COUNT  10

IVector* CVector_New()
{
    return (IVector*)NewVT(CVector, IVector);
}

void CVector_Ctor(CVector* pMe)
{
    DECLARE_PVTBL(pvt, IVector);

    CBase_Ctor((CBase*)pMe);

    VT_FUNC(pvt, CVector, Release);
    VT_FUNC(pvt, CVector, Size);
    VT_FUNC(pvt, CVector, GetAt);
    VT_FUNC(pvt, CVector, Replace);
    VT_FUNC(pvt, CVector, Insert);
    VT_FUNC(pvt, CVector, DeleteAt);
    VT_FUNC(pvt, CVector, DeleteAll);
    VT_FUNC(pvt, CVector, SetPfnFree);
    VT_FUNC(pvt, CVector, SetGrowCount);
    VT_FUNC(pvt, CVector, Reserve);

    pMe->nGrowCount = DEFAULT_GROW_COUNT;
}

static __inline void CVector_Init(CVector* pMe)
{
    pMe->ppItems = NULL;
    pMe->nCount = 0;
    pMe->cbCapacity = 0;
}

void CVector_Dtor(CVector* pMe)
{
    CVector_DeleteAll((IVector*)pMe);
}

uint32 CVector_Release(IVector* po)
{
    DECLARE_ME(CVector);

    if (CBase_Refs(po))
    {
        CVector_Dtor(pMe);
    }

    return CBase_Release((IBase*)po);
}

uint32 CVector_Size(IVector* po)
{
    DECLARE_ME(CVector);
    return pMe->nCount;
}

int CVector_GetAt(IVector* po, uint32 nIndex, void** ppe)
{
    DECLARE_ME(CVector);
    if (nIndex >= pMe->nCount)
        return EBADPARM;

    *ppe = pMe->ppItems[nIndex];
    return SUCCESS;
}

int CVector_Replace(IVector* po, uint32 nIndex, void* pNew)
{
    DECLARE_ME(CVector);
    int nErr = SUCCESS;
    void *pe = NULL;

    nErr = CVector_GetAt(po, nIndex, &pe);
    if (SUCCESS == nErr)
    {
        if (pe && pMe->pfnFree)
        {
            pMe->pfnFree(pe);
        }

        pMe->ppItems[nIndex] = pNew;
    }

    return nErr;
}

int CVector_Insert(IVector* po, uint32 nIndex, void* pe)
{
    DECLARE_ME(CVector);
    int nErr = SUCCESS;

    if (nIndex > pMe->nCount)
        nIndex = pMe->nCount;

    if (pMe->nCount * sizeof(void*) >= pMe->cbCapacity)
    {
        nErr = CVector_Reserve(po, pMe->nCount + pMe->nGrowCount);
    }
    if (SUCCESS != nErr)
    {
        return nErr;
    }

    if (nIndex < pMe->nCount)
    {
        MEMMOVE(pMe->ppItems + nIndex + 1, pMe->ppItems + nIndex, 
            (pMe->nCount - nIndex) * sizeof(void*));
    }
    pMe->ppItems[nIndex] = pe;
    pMe->nCount++;

    return nErr;
}

int CVector_DeleteAt(IVector* po, uint32 nIndex)
{
    DECLARE_ME(CVector);
    int nErr = SUCCESS;

    nErr = CVector_Replace(po, nIndex, NULL);
    if (SUCCESS != nErr)
    {
        return nErr;
    }

    MEMMOVE(pMe->ppItems + nIndex, pMe->ppItems + nIndex + 1, 
        (pMe->nCount - nIndex - 1) * sizeof(*pMe->ppItems));
    pMe->nCount--;

    if (pMe->nCount * sizeof(void*) < pMe->cbCapacity / 2)
    {
        pMe->ppItems = REALLOC(pMe->ppItems, pMe->nCount * sizeof(void*));
    }

    return nErr;
}

void CVector_DeleteAll(IVector* po)
{
    DECLARE_ME(CVector);
    uint32 i = 0;

    if (pMe->pfnFree)
    {
        for (i = 0; i < pMe->nCount; i++)
        {
            CVector_Replace(po, i, NULL);
        }
    }

    FREEIF(pMe->ppItems);
    CVector_Init(pMe);
}

PFNNOTIFY CVector_SetPfnFree(IVector* po, PFNNOTIFY pfnFree)
{
    DECLARE_ME(CVector);
    PFNNOTIFY pfnOld = pMe->pfnFree;
    pMe->pfnFree = pfnFree;
    return pfnOld;
}

void CVector_SetGrowCount(IVector* po, int n)
{
    DECLARE_ME(CVector);
    pMe->nGrowCount = n;
}

int CVector_Reserve(IVector* po, uint32 n)
{
    DECLARE_ME(CVector);
    void* pMem = NULL;

    pMem = REALLOC(pMe->ppItems, n * sizeof(void*));
    if (0 != n && NULL == pMem)
    {
        return ENOMEMORY;
    }

    pMe->ppItems = pMem;
    pMe->cbCapacity = n;

    return SUCCESS;
}
